#include <stdio.h>
#include <stdlib.h>
#include "array.h"

#define ARRAY_RAW_DATA(array) ((int*)(array) - 2)
#define ARRAY_CAPACITY(array) (ARRAY_RAW_DATA(array)[0])
#define ARRAY_OCCUPIED(array) (ARRAY_RAW_DATA(array)[1])

void* array_hold(void* array, int count, int item_size) {
	if (array == NULL) {
		// Total raw size = [2 ints] + [count × item_size] bytes
		size_t raw_size = (sizeof(int) * 2) + (item_size * count);

		// malloc returns space for metadata + array
		int* base = (int*)malloc(raw_size);
		if (!base) return NULL; // protect against malloc failure

#pragma warning(push)
#pragma warning(disable : 6386) // buffer overrun false positive
		base[0] = count;  // capacity
		base[1] = count;  // occupied
#pragma warning(pop)

		return base + 2;  // return pointer to the usable array
	}

	if (ARRAY_OCCUPIED(array) + count <= ARRAY_CAPACITY(array)) {
		ARRAY_OCCUPIED(array) += count;
		return array;
	}

	// Need to grow
	int needed_size = ARRAY_OCCUPIED(array) + count;
	int double_curr = ARRAY_CAPACITY(array) * 2;
	int capacity = (needed_size > double_curr) ? needed_size : double_curr;
	int occupied = needed_size;

	// Raw size = [2 ints] + [capacity × item_size]
	size_t raw_size = (sizeof(int) * 2) + (item_size * capacity);

	// realloc base pointer
	int* old_base = ARRAY_RAW_DATA(array);
	int* new_base = (int*)realloc(old_base, raw_size);
	if (!new_base) return array; // fail safely

#pragma warning(push)
#pragma warning(disable : 6386) // buffer overrun false positive
	new_base[0] = capacity;
	new_base[1] = occupied;
#pragma warning(pop)
	return new_base + 2;
}

int array_length(void* array) {
	return (array != NULL) ? ARRAY_OCCUPIED(array) : 0;
}

void array_free(void* array) {
	if (array != NULL) {
		free(ARRAY_RAW_DATA(array));
	}
}
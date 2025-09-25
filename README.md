# Software Rasterizer

A **software rendering engine built from scratch in C**, designed for educational clarity and low-level performance. It demonstrates the fundamentals of the 3D graphics pipeline including **triangle rasterization, Z-buffering, texture mapping, and dynamic lighting** — all without GPU acceleration. Built with **SDL2** for display and debugging.

---

## ✨ Features

- **Custom Graphics Loop**  
  `process_input → update (matrices) → project_points (perspective) → render → draw → repeat`

- **Drawing Algorithms**
  - DDA (Digital Differential Analyzer) for lines and edges  
  - Flat-top & flat-bottom triangle rasterization  

- **Mesh and Texture Loading**
  - `.obj` mesh loader (custom implementation)  
  - `.png` texture loading using lightweight library  

- **Culling & Depth**
  - Backface culling  
  - Painter’s algorithm and Z-buffer  

- **Transforms**
  - World matrix per object (translation, rotation, scaling)  
  - Projection and view matrix for camera movement  

- **Lighting & Shading**
  - Flat shading  
  - Gouraud shading  
  - Blinn-Phong lighting with dynamic sources  

- **Texture Mapping**
  - Barycentric coordinates  
  - Perspective-correct affine mapping (no distortion)  

- **Debugging**
  - SDL2 output window  
  - Toggle wireframe, shaded, or textured modes  

---

## 🗂️ Project Structure

src/
├── main.c # Entry point
├── renderer.c # Core rasterizer
├── rasterizer.c # Triangle filling
├── zbuffer.c # Depth buffer
├── mesh_loader.c # OBJ loader
├── texture_loader.c # PNG loader
├── math.c # Vector/Matrix math
└── lighting.c # Shading & lighting

---

## ⚙️ Dependencies

- [SDL2](https://www.libsdl.org/) – window & display  
- [stb_image.h](https://github.com/nothings/stb) – PNG texture loading  

---

## 🛠️ Build Instructions

### Linux / macOS
```bash
gcc src/*.c -o rasterizer -lSDL2 -lm
./rasterizer

gcc src/*.c -o rasterizer.exe -lmingw32 -lSDL2main -lSDL2 -lm
rasterizer.exe

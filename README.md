# Software Rasterizer

A **software rendering engine built from scratch in C**, designed for educational clarity and low-level performance. It demonstrates the fundamentals of the 3D graphics pipeline including **triangle rasterization, Z-buffering, texture mapping, and dynamic lighting** — all without GPU acceleration. Built with **SDL2** for display and debugging.

---

## 📸 Screenshots

![software_rendering](https://github.com/user-attachments/assets/563dc044-409e-4d78-9769-efdfd6fbc654)

---

## Current View Formats
- wireframe with Backface culling
- shaded wirframe with Backface culling
- shaded with Backface culling
- Texture wireframe with Backface culling
- Texture with Backface culling

---

## ✨ Features

- **Custom Graphics Loop**  
  `process_input → update (matrices) → project_points (perspective) → render → draw → repeat`
- **Drawing Algorithms**
  - DDA (Digital Differential Analyzer) for lines and edges
  - Flat-top & flat-bottom triangle rasterization
- **Mesh and Texture Loading**
  - `.obj` mesh loader (custom)
  - `.png` texture loading using a lightweight library
- **Culling & Depth**
  - Backface culling
  - Painter’s algorithm and Z-buffer
- **Transforms**
  - World matrix per object (translation, rotation, scaling)
  - View + projection matrices for camera
- **Lighting & Shading**
  - Flat shading
  - Gouraud shading
  - Blinn-Phong lighting with dynamic sources
- **Texture Mapping**
  - Barycentric interpolation
  - Perspective-correct mapping (no distortion)
- **Debugging**
  - SDL2 output window
  - Toggle wireframe / shaded / textured

---

## 🗂️ Project Structure

~~~
src/
├── main.c            # Entry point
├── renderer.c        # Core rasterizer
├── rasterizer.c      # Triangle filling
├── zbuffer.c         # Depth buffer
├── mesh_loader.c     # OBJ loader
├── texture_loader.c  # PNG loader
├── math.c            # Vector/Matrix math
└── lighting.c        # Shading & lighting
~~~

---

## ⚙️ Dependencies

- [SDL2](https://www.libsdl.org/) — window & display
- [stb_image.h](https://github.com/nothings/stb) — PNG texture loading

---

## 🛠️ Build Instructions

### Linux / macOS
~~~bash
gcc src/*.c -o rasterizer -lSDL2 -lm
./rasterizer
~~~

### Windows (MinGW)
~~~bash
gcc src/*.c -o rasterizer.exe -lmingw32 -lSDL2main -lSDL2 -lm
rasterizer.exe
~~~

---

## 🎮 Controls

- **Arrow Keys / WASD** — Move or rotate camera
- **R / T / Y** — Scale object
- **1–3** — Toggle shading (Flat, Gouraud, Blinn-Phong)
- **4** — Wireframe mode
- **Esc** — Quit

---

## 🚀 Roadmap

- [ ] Mipmapping for textures
- [ ] Normal mapping
- [ ] Multi-light support
- [ ] SIMD optimizations

---

*This project has been created as part of the 42 curriculum by moazzedd.*

# SCOP — Introduction to GPU Rendering

## Description

SCOP is a 42 school project focused on introduction to GPU rendering using OpenGL.
The goal is to build a small program that loads a 3D object from a `.obj` file, parses it manually, and renders it in real time using OpenGL 3.3 Core Profile.

The program displays the 3D object in perspective projection, allows the user to rotate and translate it along its three main axes, and supports toggling between a colored view (faces in distinct shades of gray) and a textured view, with a smooth transition between the two.

All rendering-related code is written from scratch: matrix math, `.obj` parser, shader loader, and texture loader — no external library is used for these parts.

## Features

- `.obj` file parser (vertices, faces)
- Perspective projection (objects farther away appear smaller)
- Rotation on X, Y, Z axes
- Translation on X, Y, Z axes
- Per-face coloring with shades of gray
- BMP texture loader (no external library)
- Smooth color ↔ texture transition via `uBlend` uniform
- Cross-platform: macOS and Linux (no `sudo` required on Linux)

## Instructions

### Dependencies

Install all required dependencies with a single command:

```bash
make deps
```

- **macOS** : installs GLFW via Homebrew
- **Linux** : clones and compiles GLFW locally (no sudo required)

GLAD (OpenGL function loader) is already included in `include/glad/`.

### Compilation

```bash
make
```

### Usage

```bash
./scop <file.obj>
```

Example:
```bash
./scop assets/42.obj
./scop assets/cube.obj
```

### Controls

| Key | Action |
|---|---|
| `A` / `D` | Rotate around Y axis |
| `W` / `S` | Rotate around X axis |
| `Q` / `E` | Rotate around Z axis |
| `←` / `→` | Translate on X axis |
| `↑` / `↓` | Translate on Y axis |
| `+` / `-` | Translate on Z axis |
| `T` | Toggle texture (smooth transition) |
| `R` | Reset position |
| `Escape` | Quit |

### Cleanup

```bash
make clean      # remove object files
make fclean     # remove object files + binary
make distclean  # fclean + remove locally compiled libs (Linux)
make re         # full recompile
```

## Technical Choices

- **Language** : C (c11)
- **Graphics API** : OpenGL 3.3 Core Profile
- **Window/Events** : GLFW (only authorized external library)
- **GL Loader** : GLAD (included as source files, not an external library)
- **Texture format** : BMP 24-bit (parsed manually, supports both standard and top-down BMP)
- **Matrix math** : custom `mat4` and `vec3` implementation (column-major, compatible with OpenGL)
- **Projection** : perspective matrix built from scratch (`mat4_perspective`)
- **Camera** : look-at matrix built from scratch (`mat4_look_at`)
- **Object centering** : geometric center computed from bounding box, applied before rotation so the object spins around its own axis

## Project Structure

```
scop/
├── Makefile
├── README.md
├── assets/          # .obj files and texture
├── shaders/
│   ├── vertex.glsl
│   └── fragment.glsl
├── include/
│   ├── glad/        # GLAD OpenGL loader
│   └── KHR/
└── src/
    ├── main.c
    ├── window.c/h   # GLFW window + input handling
    ├── shader.c/h   # GLSL shader compiler + linker
    ├── mesh.c/h     # GPU mesh (VAO/VBO) builder
    ├── obj_parser.c/h
    ├── texture.c/h  # BMP texture loader
    └── math/
        ├── vec3.c/h
        └── mat4.c/h
```

## Resources

- [OpenGL Documentation (docs.gl)](https://docs.gl) — reference for all OpenGL functions
- [LearnOpenGL](https://learnopengl.com) — tutorials on shaders, textures, transformations
- [The OBJ file format specification](http://paulbourke.net/dataformats/obj/) — complete .obj format reference
- [Perspective Projection Matrix](https://www.songho.ca/opengl/gl_projectionmatrix.html) — math behind perspective projection
- [GLFW Documentation](https://www.glfw.org/docs/latest/) — window and input management
- [Column-major matrices in OpenGL](https://www.opengl.org/archives/resources/faq/technical/transformations.htm)

### AI Usage

Claude (claude.ai) was used for guidance on the initial project structure and Makefile setup. All rendering code, mathematics, parser, and shader logic were written and understood manually as part of the learning process.
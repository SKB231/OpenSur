# OpenSur — Claude Setup Instructions

## Before starting any task

Run these checks automatically at the start of each session:

### 1. Initialize submodules

If `vendor/imgui` or `Sponza` directories are empty:

```bash
git submodule update --init --recursive
```

### 2. Install dependencies

**macOS (Homebrew):**
```bash
brew install glfw glm
```

**Linux (apt):**
```bash
sudo apt install libglfw3-dev libglm-dev
```

## Building

```bash
cmake -S . -B ./build
cmake --build ./build
./build/main
```

## Project structure

- `main.cpp` — entry point
- `src/` — shader, camera, model, mesh implementations
- `include/` — project headers (`Shader/`, `Camera/`, `Model/`, `Mesh/`)
- `vendor/` — third-party: imgui (git submodule), assimp (prebuilt dylib), glad
- `Sponza/` — Sponza scene assets (git submodule)
- `shaders/` — GLSL vertex/fragment shaders

## Known platform notes

- On macOS, glfw and glm are expected at `/opt/homebrew/opt/glfw` and `/opt/homebrew/opt/glm` (arm64 Homebrew default). CMakeLists.txt handles this in the `if(APPLE)` block.
- `vendor/assimp/libassimp.5.4.3.dylib` is a prebuilt binary for arm64 macOS. Linux builds will need assimp installed separately.

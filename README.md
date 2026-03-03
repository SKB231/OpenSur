# OpenSur

An OpenGL renderer built following [LearnOpenGL](https://learnopengl.com), featuring model loading, lighting, and an ImGui debug UI.

<p align="center">
  <img src="https://github.com/user-attachments/assets/746dc328-2286-401e-ae7c-ddf9b2af2d80" width="400" alt="Screenshot 1">
  <img src="https://github.com/user-attachments/assets/f82222d0-0030-4c6b-9761-ee47d61bd0ad" width="400" alt="Screenshot 2">
  <img src="https://github.com/user-attachments/assets/08392a31-6644-4b2c-80cb-d65ae9a55660" width="400" alt="Screenshot 3">
</p>

## Dependencies

**macOS:**
```bash
brew install glfw glm
```

**Linux:**
```bash
sudo apt install libglfw3-dev libglm-dev
```

## Setup

Clone with submodules:
```bash
git clone --recurse-submodules <repo-url>
```

Or if already cloned:
```bash
git submodule update --init --recursive
```

## Build & Run

```bash
cmake -S . -B ./build
cmake --build ./build
./build/main
```

## Notes

- Tested on macOS (Apple M1) and Linux
- Assimp is bundled as a prebuilt arm64 dylib (`vendor/assimp`); Linux users will need to install assimp separately

# [LearnOpenGL](https://learnopengl.com/Getting-started/Hello-Triangle)

This uses a CMake build. It works with Linux and OSX (M1 2020), setup the build files with (Once your terminal is in the project dir):
Make sure to also have the GLFW, glm, and the glad libraries, as the current CMake assumes you have the libraries in your system. I installed my libraries from [https://shnoh171.github.io/gpu%20and%20gpu%20programming/2019/08/26/installing-glfw-on-ubuntu.html](here) (GLFW and glad), and installing `libglm-dev` via apt (for the math GLM library).


```
cmake -S . -B ./build
```

To compile and run:

```
cmake --build ./build

./build/main
```

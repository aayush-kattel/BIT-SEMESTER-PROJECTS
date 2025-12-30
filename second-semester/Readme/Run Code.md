g++ -std=c++11 -o pacman pacman.cpp -lGL -lGLU -lglut -lSDL2 -lSDL2_mixer -lpng -lz

## Command Breakdown:

# g++ - The C++ Compiler
- GNU C++ compiler - converts your C++ code to executable

# -std=c++11 - C++ Version
- C++11 standard - enables modern C++ features
- Your code uses C++11 features like std::vector methods

# -o pacman - Output File
- Creates executable named pacman
- Without this, it would create a.out (default)

# pacman.cpp - Your Source Code
- Main game file containing all your Pacman code

## Library Links (What makes your game work):
# OpenGL Graphics:
-lGL - OpenGL core library (3D/2D graphics)
-lGLU - OpenGL Utilities (helpers for OpenGL)
-lglut - GLUT toolkit (window management, input)

# SDL2 Multimedia:
-lSDL2 - Simple DirectMedia Layer (window, input, audio)
-lSDL2_mixer - SDL audio mixing (sound effects, music)

# PNG Image Loading:
-lpng - PNG image library (loads PNG textures)
-lz - Compression library (required by PNG)
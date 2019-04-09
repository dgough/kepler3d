# Kepler3D

<img align="right" width="40%" height="40%" src="https://i.imgur.com/9hK19jE.jpg" />

Kepler3D is an OpenGL and C++14 game engine.

This is a learning experience and a work in progress so I don't recommend using this engine in its current state.

**Features:**

- OpenGL 4.2, C++14
- [glTF 2.0](https://github.com/KhronosGroup/glTF) scene loader and renderer
- Bitmap font loader and renderer (using the [Angel Code format](http://www.angelcode.com/products/bmfont/))
- Uses [GLFW](http://www.glfw.org/) for platform abstraction
- Unit tests using [google test](https://github.com/google/googletest)

# Building

## Windows

<img align="right" width="40%" height="40%" src="https://i.imgur.com/BKiBd1L.jpg" />

**Requirements:**

- 64 bit Windows
- Visual Studio 2019

External-dependencies and resource files are stored separately to keep the repo size down.

### Setup

- Clone the repo
- Download [kepler3d_deps.zip](https://github.com/dgough/kepler3d/releases/download/v0.1.0/kepler3d_deps.zip) from [releases](https://github.com/dgough/kepler3d/releases) and unzip to the kepler3d directory
- Open **kepler3d.sln**

# FAQ

### Should I use this engine to make a game?

No. It is very incomplete and unoptimized.

### Are you accepting outside contributions?

No. I want to stay as the sole contributor for now.

# Dependencies

- [GLFW](http://www.glfw.org/)
- [glad](https://github.com/Dav1dde/glad)
- [glm](http://glm.g-truc.net)
- [SOIL2](https://bitbucket.org/SpartanJ/soil2)
- [RapidJSON](https://github.com/miloyip/rapidjson)
- [libb64](http://libb64.sourceforge.net)
- [google test](https://github.com/google/googletest)

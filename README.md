# planarviz
Lightweight OpenGL &amp; SDL2 iteractive visualization of 2D geometry, with ImGui widgets

## Installation & Usage (Unix Systems (macOS, Ubuntu, etc., ...))

Clone the project into your computer:

```bash
git clone https://github.com/Mickelbil84/planarviz.git
cd planarviz
```

Once you have prerequisities installed (see below), you can simply run:

```bash
mkdir build; cd build; cmake ..; cd ..
chmod a+x ./scripts/build.sh
chmod a+x ./scripts/install.sh
./scripts/build.sh
sudo ./scripts/install.sh
```

Then, in your own project, simply add a `CMakeLists.txt` file with the following:

```CMake
cmake_minimum_version(3.20)
project(myproject)

find_package(planarviz REQUIRED)

add_executable(myexec main.cpp)
target_link_libraries(myexec PRIVATE planarviz::planarviz)
```

See TODO for example.

## Prerequisites

### macOS

Tested on Apple Silicon, on Sonoma 14.1.

1. Install XCode from the AppStore (C++ build tools also suffice)
1. Install Homebrew at [https://brew.sh/](https://brew.sh/)
2. Run:
    ```bash
    brew install cmake fmt glew sdl2 cgal
    ```
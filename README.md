# wincpp
A fully featured x64 Win32 wrapper written in modern C++. Wincpp's goal is to provide a simple and easy to use C++ interface for communicating with the Windows OS. This project is still in early development so expect frequent changes to the main branch.

## Installation
This is a [CMake](https://cmake.org/) project, so we've created a convenient interface for installing it using [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html). If you aren't using CMake, you can download any of the prebuilt static binaries from any of the latest releases.

If you are using CMake, simply add the following lines to your `CMakeLists.txt` file:
```cmake
include (FetchContent) # if you don't have this already

# Fetch the latest version
FetchContent_Declare (wincpp URL https://github.com/atrexus/wincpp/releases/latest/download/wincpp-src.zip)
FetchContent_MakeAvailable (wincpp)

# Link the SDK into your project
target_link_libraries(your_project PRIVATE wincpp)
```
## Documentation
To get started with Wincpp, check out the offcial [Wiki](https://github.com/atrexus/wincpp/wiki) of this GitHub repository. It contains detailed documentation for the different interfaces and simple tutorials to help you get started.

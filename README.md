# wincpp

A lightweight header-only x64 Win32 wrapper written in modern C++23. Wincpp provides a small C++ interface for working with Windows processes,
modules, memory, threads, and windows while keeping Windows SDK includes isolated behind guarded implementation headers.

## Installation

This is a CMake project and can be consumed with `FetchContent`.

```cmake
include(FetchContent)

FetchContent_Declare(wincpp URL https://github.com/atrexus/wincpp/releases/latest/download/wincpp-src.zip)
FetchContent_MakeAvailable(wincpp)

target_link_libraries(your_project PRIVATE wincpp::wincpp)
```

The library is header-only, so no static or shared library binary is required.

## Documentation

To get started, check out the official [Wiki](https://github.com/atrexus/wincpp/wiki) of this GitHub repository. It contains detailed
documentation for the different interfaces and simple tutorials to help you get started.

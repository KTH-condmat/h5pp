[![Ubuntu 16.04](https://github.com/DavidAce/h5pp/workflows/Ubuntu%2016.04/badge.svg?branch=master)](https://github.com/DavidAce/h5pp/actions)
[![Ubuntu 20.04](https://github.com/DavidAce/h5pp/workflows/Ubuntu%2020.04/badge.svg?branch=master)](https://github.com/DavidAce/h5pp/actions)
[![Windows 10](https://github.com/DavidAce/h5pp/workflows/Windows%2010/badge.svg?branch=master)](https://github.com/DavidAce/h5pp/actions)
[![MacOS 10.15](https://github.com/DavidAce/h5pp/workflows/macOS%2010.15/badge.svg?branch=master)](https://github.com/DavidAce/h5pp/actions)
[![Documentation Status](https://readthedocs.org/projects/h5pp/badge/?version=latest)](https://h5pp.readthedocs.io/en/latest/?badge=latest)
[![Conan](https://img.shields.io/badge/Install%20with-conan-green)](https://conan.io/center/h5pp)
[![OS](https://img.shields.io/badge/OS-Linux%7COSX%7CWindows-blue)](https://img.shields.io/badge/OS-Linux%7COSX%7CWindows-blue)
[![codecov](https://codecov.io/gh/davidace/h5pp/branch/dev/graph/badge.svg)](https://codecov.io/gh/davidace/h5pp)
---

# h5pp

`h5pp` is a high-level C++17 interface for the [HDF5](https://www.hdfgroup.org/) C library. With simplicity in
mind, `h5pp` lets users store common C++ data types into portable binary [HDF5](https://www.hdfgroup.org/) files.

[Latest release](https://github.com/DavidAce/h5pp/releases)

[Documentation](https://h5pp.readthedocs.io)

Go to [examples](https://github.com/DavidAce/h5pp/tree/master/examples) to learn how to use `h5pp`.

Go to [quickstart](https://github.com/DavidAce/h5pp/tree/master/quickstart) to see ways of installing `h5pp`.

---

## Table of Contents

* [Introduction](#introduction)
* [Features](#features)
* [Examples](#examples)
* [Get h5pp](#get-h5pp)
* [Requirements](#requirements)
* [Install](#install)
* [To-do](#to-do)

## Introduction

[HDF5](https://www.hdfgroup.org/) is a portable file format for storing large datasets efficiently. With
official [low-level API's for C and Fortran](https://portal.hdfgroup.org/display/HDF5/Core+Library), wrappers for C++
and Java and third-party bindings to Python, Julia, Matlab and many others, HDF5 is a great tool for handling data
in a collaborative setting.

Although well documented, the low-level C API is vast and using it directly can be challenging. There are many
high-level wrappers already that help the user experience, but as a matter of opinion, things could be even simpler.

`h5pp` is a high-level C++17 interface for the HDF5 C library which aims to be simple to use:

* Read and write common C++ types in a single line of code.
* No prior knowledge of HDF5 is required.
* Meaningful logs and error messages.
* Use HDF5 with modern, idiomatic, type-safe C++.
* Simple options common tasks, e.g. for file access, storage layout, hyperslabs, chunking and compression.
* Simple installation with modular dependencies and opt-in automation.
* Simple documentation (work in progress).

## Features

* Header-only C++17 template library.
* High-level front-end to the C API of the HDF5 library.
* CMake installation of `h5pp` and its dependencies (opt-in).
* Multi-platform: Linux, Windows, OSX. (Developed under Linux).
* Type support:
    * all numeric types: `(u)int#_t`, `float`, `double`, `long double`.
    * **`std::complex<>`** with any of the types above.
    * CUDA-style POD-structs with `x,y` or `x,y,z` members as atomic type, such as `float3` or `double2`. These work
      with any of the types above. In `h5pp` these go by the name `Scalar2<>` and `Scalar3<>`.
    * Contiguous containers with a `.data()` member, such as `std::vector<>`.
    * Raw C-style arrays or pointer to buffer + dimensions.
    * [**Eigen**](http://eigen.tuxfamily.org) types such as `Eigen::Matrix<>`, `Eigen::Array<>` and `Eigen::Tensor<>`,
      with automatic conversion to/from row-major storage
    * Text types `std::string`, `char` arrays, and `std::vector<std::string>`.
    * Structs as HDF5 Compound types ([example](https://github.com/DavidAce/h5pp/blob/master/examples/example-04a-custom-struct-easy.cpp))
    * Structs as HDF5 Tables (with user-defined compound HDF5 types for entries)

## Examples

Using `h5pp` is intended to be simple. After initializing a file, most the work can be achieved using just two member
functions `.writeDataset(...)` and `.readDataset(...)`.

### Write an `std::vector`

```c++
    #include <h5pp/h5pp.h>
    int main() {
        std::vector<double> v = {1.0, 2.0, 3.0};    // Define a vector
        h5pp::File file("somePath/someFile.h5");    // Create a file 
        file.writeDataset(v, "myStdVector");        // Write the vector into a new dataset "myStdVector"
    }
```

### Read an `std::vector`

```c++
    #include <h5pp/h5pp.h>
    int main() {
        h5pp::File file("somePath/someFile.h5", h5pp::FileAccess::READWRITE);    // Open (or create) a file
        auto v = file.readDataset<std::vector<double>>("myStdVector");           // Read the dataset from file
    }
```

Find more code examples in the [examples directory](https://github.com/DavidAce/h5pp/tree/master/examples).


## Get h5pp

There are currently 3 ways to obtain `h5pp`:

* From [conan-center](https://conan.io/center/h5pp).
* From [GitHub](https://github.com/DavidAce/h5pp).
* As a `.deb` package from [latest release](https://github.com/DavidAce/h5pp/releases) (Ubuntu/Debian only).

## Requirements

* C++17 capable compiler. GCC version >= 7 or Clang version >= 7.0
* CMake version >= 3.15
* [**HDF5**](https://support.hdfgroup.org/HDF5/)  library, version >= 1.8

### Optional dependencies

* [**Eigen**](http://eigen.tuxfamily.org) >= 3.3.4: Store Eigen containers. Enable with `#define H5PP_USE_EIGEN3`.
* [**spdlog**](https://github.com/gabime/spdlog) >= 1.3.1: Logging library. Enable with `#define H5PP_USE_SPDLOG`.
* [**fmt**](https://github.com/fmtlib/fmt) >= 6.1.2: String formatting (used in `spdlog`). Enable with `#define H5PP_USE_FMT`.

**NOTE:** Logging works the same with or without [Spdlog](https://github.com/gabime/spdlog) enabled. When Spdlog is *
not* found, a hand-crafted logger is used in its place to give identical output but without any performance
considerations (implemented with STL lists, strings and streams).

## Install

Read the instructions [here](https://h5pp.readthedocs.io/en/latest/installation.html#installation) or see installation
examples under [quickstart](https://github.com/DavidAce/h5pp/tree/master/quickstart). Find a summary below.

### Option 1: Install with Conan (Recommended)

Install and configure [conan](https://conan.io), then run the following command to install
from [conan center](https://conan.io/center/h5pp):

```
> conan install h5pp/1.10.0 --build=missing
```

### Option 2: Install with CMake

Git clone and build from command line:

```bash
    git clone https://github.com/DavidAce/h5pp.git
    mkdir h5pp/build
    cd h5pp/build
    cmake -DCMAKE_INSTALL_PREFIX=<install-dir>  ../
    make
    make install
```

Read more about `h5pp` CMake options in the [documentation](https://h5pp.readthedocs.io/en/latest/installation.html)

### Option 3: Copy the headers

`h5pp` is header-only. Copy the files under `include` to your project and then add `#include <h5pp/h5pp.h>`.

Read more about linking h5pp to its dependencies [here](https://h5pp.readthedocs.io/en/latest/installation.html#link)

## To-do

* For version 2.0.0
    * Single header
    * Compiled-library mode

In no particular order

* Continue adding documentation
* Expand the pointer-to-data interface
* Expand testing using catch2 for more edge-cases in
    * filesystem permissions
    * user-defined types
    * tables
* Expose more of the C-API:
    * More support for parallel read/write with MPI

  

[![Build Status](https://travis-ci.com/TimGuite/workflow_challenge.svg?branch=master)](https://travis-ci.com/TimGuite/workflow_challenge)

# Workflow Challenge

A small C++ package intended to implement a solution to the challenges outlined [here](CHALLENGE.md).

## Template

This package is structured according to the template developed at https://github.com/OxfordRSE/template-project-cpp and therefore maintains it's license.

## Testing

Testing is provided by the [Catch2](https://github.com/catchorg/Catch2/blob/master/single_include/catch2/catch.hpp) header only unit test module.

## Overview

I have tried to conform to common software engineering 'best practices', and to that end this repository features the following:

- C++20
- GCC 9.2.1, G++9.2.1
- [CMake 3.17.0](https://cmake.org/) build script for cross-platform configuration (see 
  [here](#cmakeliststxt) for more info)
- [Catch2](https://github.com/catchorg/Catch2) unit testing framework
- [Travis](https://travis-ci.org/) integration for automated testing on Linux 
  and macOS (see [here](travis.yml))
- [Codecov](https://codecov.io/) integration for automated coverage testing (see 
  [here](#codecov))
- [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) for automated 
  source code formatting (see [here](#clang-format))
- [Clang-Tidy](http://clang.llvm.org/extra/clang-tidy/) for static analysis to 
  catch coding errors (see [here](#clang-tidy))
- [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html) for 
  catching memory-related issues (see [here](#addresssanitizer))

# Getting Started

- Install [CMake.17.0](https://cmake.org/download/) and a C++ compiler.
CMake 3.17.0 is the latest version so may not be available directly from your system's packaging repository.
See [here](#CMakeLists.txt) for further instructions on installing.

- Make a build directory and configure the project. E.g. Assuming you are in a 
  directory containing the template source code:

```bash
$ mkdir build
$ cd build
$ cmake ..
```

- Compile the project

```bash
$ make
```

- Run all the tests using CTest (this is bundled with CMake)

```bash
$ ctest --output-on-failure
```

- Run the tests directly:

```bash
./mytest
```

## Executables

### Synchronous Executor

```bash
./syncCli
```

### Asynchronous Executor

You will not in the asynchronous version that steps C and D should run truly in parallel, as expected.

```bash
./asyncCli
```

# Tools

### CMakeLists.txt

[CMake](https://cmake.org/) is a cross-platform build configuration tool, which 
generates compilation scripts (e.g. using `Makefiles` in linux, or Visual Studio 
projects for windows) that you can run to compile your C++ project.

Useful links for installing CMake 3.17.0:

- Install:  https://anglehit.com/how-to-install-the-latest-version-of-cmake-via-command-line/
- Travis: https://github.com/travis-ci/travis-ci/issues/7437
- Missing library issue: https://stackoverflow.com/questions/16248775/cmake-not-able-to-find-openssl-library

### .travis.yml

This template uses [Travis CI](https://docs.travis-ci.com/) for automated 
testing. This is fully defined in the `.travis.yml` file and includes:

- A matrix of build configurations: 
    - Ubuntu 18.04 with GCC 9
- Test coverage data collection with 
  [lcov](http://ltp.sourceforge.net/coverage/lcov.php) and reporting with 
  [codecov](https://codecov.io/) 

The results of the automated testing are reported on the [Travis CI project 
page](https://travis-ci.org/OxfordRSE/template-project-cpp)
  
### Code coverage

There are some known issues currently using code coverage tools with CMake 3.17.0.
It is expected that these will be amended in due course, a which point the methodology below will be put into place.
At the moment, it is safe to ignore this section.

Code coverage allow you to see how much of your code is actually covered by your 
test suite. That is, which lines of code are actually run during the tests. 

To do this, the code must be compiled with compiler flags that allow the 
executable to collect information about which lines of code are called during 
execution. For GCC or Clang this means adding the `--coverage` flag, which is 
done in the 
[`CMakeLists.txt`](https://github.com/OxfordRSE/template-project-cpp/blob/master/CMakeLists.txt) 
configuration file.

You can see the 
[.travis.yml](https://github.com/OxfordRSE/template-project-cpp/blob/master/.travis.yml) 
for an example of how to collect the coverage data in a readable format using 
[lcov](http://ltp.sourceforge.net/coverage/lcov.php) (see the "after_success:" 
section). This data is then sent to 
[codecov](https://codecov.io/gh/OxfordRSE/template-project-cpp) for an even 
easier-to-read web interface.

### .clang-format

[Clang Format](https://clang.llvm.org/docs/ClangFormat.html) is a tool to 
automatically format your code according to a set style configuration. This is 
incredibly useful because it frees you up from having to worry about things like 
indenting, spacing out your code or breaking long lines of code, just type in 
your code and let Clang Format clean it up for you.

You can install Clang Format on Ubuntu using `apt`:

```bash
$ sudo apt install clang-format
```

You can set the particular style you wish to apply using the 
[.clang-format](https://github.com/OxfordRSE/template-project-cpp/blob/master/.clang-format) 
configuration file. The [clang-format 
configurator](https://zed0.co.uk/clang-format-configurator/) is a useful web app 
for generating this file.

Note that most IDEs will allow you to automatically run Clang Format when the 
file is saved, which even saves you from manually running the tool in the first 
place.

### .clang-tidy

[Clang Tidy](http://clang.llvm.org/extra/clang-tidy/) is a clang-based C++ 
linter tool. A *linter* will analyzing your code to check for common programming 
bugs and stylistic errors. This might seem similar to the warnings often given 
by the compiler, but a linter will have a much more comprehensive set of tests 
that examines the *structure* of your code rather than the often line-by-line 
checking done by the compiler.  

You can install Clang Tidy on Ubuntu using `apt`:

```bash
$ sudo apt install clang-tidy
```

The 
[.clang-tidy](https://github.com/OxfordRSE/template-project-cpp/blob/master/.clang-tidy) 
configuration file allows you to set or turn off individual or sets of checks 
done by clang-tidy. We also setup the 
[CMakeLists.txt](https://github.com/OxfordRSE/template-project-cpp/blob/master/CMakeLists.txt) 
file so that clang-tidy is run automatically during compile-time.

### AddressSanitizer

There is an optional component enabled via CMake that can use the [LLVM AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html) to detect memory errors.
This is turned on by default for the clang builds on Travis, so you will see any errors on there if it's configured.

You can also run it yourself, provided you are using the clang compiler, by using the `Template_MEMCHECK` option when running CMake.
Simply enable the option, then configure, build, and test:

```bash
cmake -DTemplate_MEMCHECK=TRUE /path/to/project
make
ctest
```
The test will fail at the first error.
It is unlikely that you will encounter a false positive when using the address sanitizer, so if you do see an error, best not to ignore it!

## Feedback and suggestions

If you have any feedback or suggestions about this project, please get in touch or open an issue.

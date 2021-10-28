# DigitalCurling-AI-example

Examples of creating AI on Digital Curling

## Building

1. Clone this repository
1. To set up submodules, execute `git submodule update --init --recursive`
1. Install [Boost](https://www.boost.org/)
1. Install [CMake](https://cmake.org/)
1. Ensure CMake is in the user `PATH`
1. Execute the following commands

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBOX2D_BUILD_TESTBED=OFF ..
cmake --build . --config RelWithDebInfo
```

:warning: If CMake could not find Boost, set the environment variable `BOOST_ROOT` to the directory in which Boost installed.

## ex0-template

デジタルカーリングAIプログラムのテンプレートです．

## ex1-stdio

ショットの初速と回転を標準入力から手入力できるようにしたものです．

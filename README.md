# DigitalCurling-AI-example
Digital Curling AI example

## Building

1. Clone this repository
   - :warning: To clone submodules together, use `git clone --recursive <URL>` instead of `git clone <URL>`
1. Install [Boost](https://www.boost.org/)
1. Set the environment variable `BOOST_ROOT` to the directory in which Boost installed
1. Install [CMake](https://cmake.org/)
1. Ensure CMake is in the user `PATH`
1. Execute the following commands

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBOX2D_BUILD_TESTBED=OFF ..
cmake --build . --config RelWithDebInfo
```

## template

デジタルカーリングAIプログラムのテンプレートです．

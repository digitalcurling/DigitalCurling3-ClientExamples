# DigitalCurling-AI-example

このリポジトリはデジタルカーリングAIの作成例を提供します．

## ビルド

1. このリポジトリをクローンします．
1. サブモジュールをセットアップするために， `git submodule update --init --recursive` を実行します．
1. [Boost](https://www.boost.org/)をインストールします．
3. [CMake](https://cmake.org/)をインストールします．
4. CMakeがユーザーの `PATH` に入っていることを確認します．
5. 下記のビルドコマンドを実行します．

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBOX2D_BUILD_TESTBED=OFF ..
cmake --build . --config RelWithDebInfo
```

## ex0-template

デジタルカーリングAIプログラムのテンプレートです．

## ex1-stdio

ショットの初速と回転を標準入力から手入力できるようにしたものです．

# DigitalCurling3-ClientExamples

このリポジトリはデジタルカーリング思考エンジンクライアントの作成例を提供します．

## ビルド

1. このリポジトリをクローンします．
1. サブモジュールをセットアップするために， `git submodule update --init --recursive` を実行します．
1. [Boost](https://www.boost.org/)をインストールします．
1. [CMake](https://cmake.org/)をインストールします．
1. CMakeがユーザーの `PATH` に入っていることを確認します．
1. 下記のビルドコマンドを実行します．

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

:warning: CMakeがBoostを見つけられない場合は，環境変数`BOOST_ROOT`にBoostをインストールしたディレクトリを設定してください．

## 思考エンジンクライアント作成例

### template

思考エンジンのテンプレートです．
`template.cpp` のTODOと書いてある箇所を編集することで思考エンジンを作成することができます．

### stdio

ショットの初速と回転を標準入力から入力できるようにしたものです．

### rulebased

簡単なif-thenルールに従い，1ターン先のシミュレーションを行う思考エンジンです．

シミュレータFCV1について初速を逆算する関数が含まれます．

## ライセンス

The Unlicense
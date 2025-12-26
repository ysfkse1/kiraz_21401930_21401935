#!/bin/bash -xe

mkdir -p build
cd build

# Jenkins'te pkg-config varsa otomatik bulur, yoksa /opt/local kullanır
cmake .. \
    -DFL_LIBRARY=/opt/homebrew/opt/flex/lib/libfl.a \
    -DFL_INCLUDE_DIR=/opt/homebrew/opt/flex/include \
    -DBISON_EXECUTABLE=/opt/homebrew/opt/bison/bin/bison \
    -DKIRAZ_TEST_WASMGEN=ON \
    -DKIRAZ_TEST_WASMGEN_MOZJS=ON

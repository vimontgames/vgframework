#!/usr/bin/env bash

set -e

cd $(dirname $0)/..
ROOT_DIR=$(pwd)

TMP_DIR=$ROOT_DIR/tmp
mkdir -p $TMP_DIR

PROJECT_DIR=$ROOT_DIR/sharpmake

echo "ROOT_DIR: $ROOT_DIR"
echo "PROJECT_DIR: $PROJECT_DIR"
echo "TMP_DIR: $TMP_DIR"

cd $TMP_DIR
if [ ! -d sharpmake/bin ]; then
    if [ ! -d sharpmake/src ]; then
        git clone https://github.com/vimontgames/Sharpmake.git sharpmake/src
    fi

    cd $TMP_DIR/sharpmake/src
    git pull origin main
    dotnet build Sharpmake.sln -c Release
    cp -r Sharpmake.Application/bin/Release/net6.0 ../bin
fi

SHARPMAKE_BIN=$TMP_DIR/sharpmake/bin/Sharpmake.Application

cd $PROJECT_DIR
$SHARPMAKE_BIN  "/sources(@'main.sharpmake.cs')"

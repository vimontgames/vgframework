#!/usr/bin/env bash

nix_setup()
{
    sh <(curl -L https://nixos.org/nix/install) --no-daemon
    . /home/codespace/.nix-profile/etc/profile.d/nix.sh
    nix-shell -p clang clang-tools
}

set -e

cd $(dirname $0)/..
ROOT_DIR=$(pwd)

TMP_DIR=$ROOT_DIR/tmp

echo "ROOT_DIR: $ROOT_DIR"
echo "TMP_DIR: $TMP_DIR"
g++ --version

cd $ROOT_DIR
files=$(find src/core -name "*.cpp")

mkdir -p $TMP_DIR
cd $TMP_DIR

compile()
{
    echo "Compiling $1"

    g++ \
        -std=c++17 \
        -Wno-cast-user-defined \
        -Wno-invalid-offsetof \
        -I${ROOT_DIR}/src \
        -I${ROOT_DIR}/extern \
        -I${ROOT_DIR}/extern/hlslpp/include \
        -I${ROOT_DIR}/extern/fmt/include/fmt \
        -I${ROOT_DIR}/extern/fmt/include \
        -c ../$1
}

## FAILURE
#compile src/core/Core.cpp
#compile src/core/File/File.cpp
#compile src/core/Logger/Logger.cpp
#compile src/core/Memory/MemoryAllocator.cpp
#compile src/core/Misc/BitMask/BitMask.cpp
#compile src/core/Misc/Random/Random.cpp
#compile src/core/Plugin/Plugin.cpp
#compile src/core/Scheduler/Scheduler.cpp
#compile src/core/string/string.cpp

## SUCCESS
#compile src/core/CmdLine/CmdLine.cpp
#compile src/core/Component/Behaviour/Behaviour.cpp
#compile src/core/Component/Component.cpp
#compile src/core/Curve/Curve.cpp
#compile src/core/GameObject/GameObject.cpp
#compile src/core/Instance/Instance.cpp
#compile src/core/Math/Math.cpp
#compile src/core/Misc/AABB/AABB.cpp
#compile src/core/Model/Model.cpp
#compile src/core/Object/DynamicProperties/DynamicProperties.cpp
#compile src/core/Object/Factory.cpp
#compile src/core/Object/Object.cpp
#compile src/core/Options/Options.cpp
#compile src/core/Precomp.cpp
#compile src/core/Resource/Resource.cpp
#compile src/core/Scheduler/Job.cpp
#compile src/core/Timer/Timer.cpp
#compile src/core/UndoRedo/UndoRedo.cpp
#compile src/core/XML/XML.cpp
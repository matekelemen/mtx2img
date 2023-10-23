#!/bin/bash

scriptDir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
buildDir="$scriptDir/build"
installDir="$scriptDir/install"
buildType="Release"

if [ -d "$buildDir" ]; then
    rm -f "$buildDir/cmake_install.cmake" "$buildDir/CMakeCache.txt"
    rm -rf "$buildDir/CMakeFiles"
else
    mkdir -p "$buildDir"
fi


if ! cmake \
    "-H$scriptDir"                              \
    "-B$buildDir"                               \
    "-DCMAKE_INSTALL_PREFIX:STRING=$installDir" \
    "-DCMAKE_BUILD_TYPE:STRING=$buildType"         \
    ; then
    exit 1
fi

if ! cmake --build "$buildDir" --config "$buildType" --target install; then
    exit 1
fi

exit 0

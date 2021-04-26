#!/bin/bash

pip install conan
source ~/.profile

# conan profile new goodok-profile --detect  # Generates default profile detecting GCC and sets old ABI
# conan profile update settings.compiler.version=8 goodok-profile
# conan profile update settings.compiler.libcxx=libstdc++11 default  # Sets libcxx to C++11 ABI

# conan install -s build_type=Release .. --build=missing
# conan install .. --profile goodok-profile

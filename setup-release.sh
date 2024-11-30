#!/bin/bash
./tools/setup-common.sh

build_dir=${1:-build-release}
build_type=Release
conan_profile=${2:-scwx-linux_gcc-11}
qt_version=6.8.0
qt_arch=gcc_64
script_dir="$(dirname "$(readlink -f "$0")")"

conan install tools/conan/profiles/${conan_profile} -tf profiles

mkdir -p ${build_dir}
cmake -B ${build_dir} -S . \
	-DCMAKE_BUILD_TYPE=${build_type} \
	-DCMAKE_CONFIGURATION_TYPES=${build_type} \
	-DCMAKE_INSTALL_PREFIX=${build_dir}/${build_type}/supercell-wx \
	-DCMAKE_PREFIX_PATH=/opt/Qt/${qt_version}/${qt_arch} \
	-DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=${script_dir}/external/cmake-conan/conan_provider.cmake \
	-DCONAN_HOST_PROFILE=${conan_profile} \
	-DCONAN_BUILD_PROFILE=${conan_profile} \
	-G Ninja

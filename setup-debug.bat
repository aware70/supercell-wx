call tools\setup-common.bat

set build_dir=build-debug
set build_type=Debug
set conan_profile=scwx-win64_msvc2022
set qt_version=6.8.0
set qt_arch=msvc2022_64

conan install tools/conan/profiles/%conan_profile% -tf profiles

mkdir %build_dir%
cmake -B %build_dir% -S . ^
    -DCMAKE_BUILD_TYPE=%build_type% ^
    -DCMAKE_CONFIGURATION_TYPES=%build_type% ^
    -DCMAKE_PREFIX_PATH=C:/Qt/%qt_version%/%qt_arch% ^
    -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=external/cmake-conan/conan_provider.cmake ^
	-DCONAN_HOST_PROFILE=${conan_profile} ^
	-DCONAN_BUILD_PROFILE=${conan_profile}
pause

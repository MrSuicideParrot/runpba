#!/bin/bash

set -e

HERE="$(dirname "$0")"
BUILD_PATH="$HERE/trusted-firmware-m-runpba/build"

echo $PATH 


cmake -S trusted-firmware-m-runpba -B $BUILD_PATH \
    -DTFM_PLATFORM=arm/mps3/corstone310/fvp\
    -DTFM_TOOLCHAIN_FILE=trusted-firmware-m-runpba/toolchain_GNUARM.cmake\
    -DTFM_PROFILE=profile_medium\
    -DCMAKE_BUILD_TYPE=Debug\
    -DTEST_NS_ATTESTATION=ON\
    -DTFM_PARTITION_PACBTI_SERVICE=ON\
    -DNS_EVALUATION_APP_PATH=$HERE/ns_app\
    -DTFM_EXTRA_MANIFEST_LIST_FILES="$PWD/pacbti_service/pacbti_service_manifest_list.yml"\
    -DTFM_EXTRA_PARTITION_PATHS="$PWD/pacbti_service"
cmake --build $BUILD_PATH -- install

echo "Firmware compiled with success!"
ls $BUILD_PATH/bin/*.bin
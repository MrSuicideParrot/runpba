cmake_minimum_required(VERSION 3.21)

project(pacbti-execution-attestation LANGUAGES C CXX)


set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc")
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
set(GCC_ELF2BIN "arm-none-eabi-objcopy")
set_property(GLOBAL PROPERTY ELF2BIN ${GCC_ELF2BIN})


set(TFM_PLATFORM_LOCAL_PATH "arm/mps3/corstone310/fvp")

set(TFM_CMAKE_ARGS
    -DTFM_PROFILE=profile_medium
    -DCMAKE_BUILD_TYPE=Debug
)

set(tfm_toolchain_file "toolchain_GNUARM.cmake")

# Configuration of executable
set(EXE_SUFFIX ".axf")
set(CMAKE_EXECUTABLE_SUFFIX ${EXE_SUFFIX})

add_compile_options(-Og)

include(ExternalProject)

ExternalProject_Add(
    tf-m-build

    # Use code fetched by FetchContent
    DOWNLOAD_COMMAND    ""
    SOURCE_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/trusted-firmware-m

    USES_TERMINAL_CONFIGURE ON
    USES_TERMINAL_BUILD     ON

    BUILD_ALWAYS ON

    CMAKE_ARGS
        -D TFM_TOOLCHAIN_FILE=<SOURCE_DIR>/${tfm_toolchain_file}
        -D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        ${TFM_CMAKE_ARGS}

    PATCH_COMMAND
        ${TFM_PATCH_COMMAND}

    BUILD_BYPRODUCTS
        ${tfm_ns_interface_generated}
        ${s_veneers_generated}
)


# Copy tf-m binaries at the root
ExternalProject_Get_Property(tf-m-build BINARY_DIR)
ExternalProject_Get_Property(tf-m-build SOURCE_DIR)


add_custom_target(tfm-binaries
  BYPRODUCTS
    ${CMAKE_BINARY_DIR}/bootloader/bl2.axf
    ${CMAKE_BINARY_DIR}/secure_partition/tfm_s.axf
    ${CMAKE_BINARY_DIR}/secure_partition/tfm_s_unsigned.bin
    ${CMAKE_BINARY_DIR}/secure_partition/tfm_s_signed.bin

  COMMAND ${CMAKE_COMMAND} -E copy
    ${BINARY_DIR}/install/outputs/bl2.axf
    "${CMAKE_BINARY_DIR}/bootloader/bl2${CMAKE_EXECUTABLE_SUFFIX}"

    COMMAND ${CMAKE_COMMAND} -E copy
    ${BINARY_DIR}/install/outputs/tfm_s_signed.bin
    "${CMAKE_BINARY_DIR}/secure_partition/tfm_s_signed.bin"
)

add_dependencies(tfm-binaries tf-m-build)
#add_dependencies(tfm-ns-interface tfm-binaries)
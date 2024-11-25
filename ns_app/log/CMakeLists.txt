#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

############################# Ns Log ###########################################
add_library(tfm_log INTERFACE)
add_library(tfm_log_interface INTERFACE)
add_library(platform_common_interface_bti INTERFACE)

target_sources(tfm_log
    INTERFACE
        tfm_log_raw.c
)

target_include_directories(tfm_log_interface
    INTERFACE
        .
)

target_include_directories(platform_common_interface_bti
    INTERFACE
        ../../trusted-firmware-m/platform/ext
        ../../trusted-firmware-m/platform/ext/cmsis
        ../../trusted-firmware-m/platform/ext/common
        ../../trusted-firmware-m/platform//ext/driver
        ../../trusted-firmware-m/platform/include
)

target_link_libraries(tfm_log
    INTERFACE
        tfm_log_interface
        platform_common_interface_bti
)

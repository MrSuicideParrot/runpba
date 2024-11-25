#-------------------------------------------------------------------------------
# Copyright (c) 2020-2023, Arm Limited. All rights reserved.
# Copyright (c) 2022-2023 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)
project(tfm_ns LANGUAGES ASM C)

# For multi-core projects, the NS app can be run on a different CPU to the
# Secure code. To facilitate this, we once again reload the compiler to load the
# setting for the NS CPU. Cmake settings are directory scoped so this affects
# anything loaded from or declared in this dir.
if (EXISTS ${TARGET_PLATFORM_PATH}/preload_ns.cmake)
    include(${TARGET_PLATFORM_PATH}/preload_ns.cmake)
    tfm_toolchain_reload_compiler()
endif()

if (TFM_MULTI_CORE_TOPOLOGY)
    # Enable TFM_MULTI_CORE_NS_OS when building with tf-m-tests NS App.
    set(TFM_MULTI_CORE_NS_OS ON CACHE BOOL "Enable NS RTOS support in NS mailbox")
endif()

# In actual NS integration, NS side build should include the source files
# exported by TF-M build.
# Directly include interface folder to simplify the NS build in this demo, since
# install always occurs at the end of build.
set(INTERFACE_SRC_DIR    ${CMAKE_SOURCE_DIR}/interface/src)
set(INTERFACE_INC_DIR    ${CMAKE_SOURCE_DIR}/interface/include)

# NS interface implemented by NSPE
set(NS_INTERFACE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/../ns_interface)

# nsid_manager
set(NSID_MGR_DIR         ${NS_INTERFACE_DIR}/ns_client_ext)

############################# TF-M NS interface #####################################
add_library(tfm_api_ns STATIC)

# Include interface headers exported by TF-M
target_include_directories(tfm_api_ns
    PUBLIC
        ${INTERFACE_INC_DIR}
        ${CMAKE_BINARY_DIR}/generated/interface/include
        $<$<BOOL:${TFM_PARTITION_NS_AGENT_MAILBOX}>:${INTERFACE_INC_DIR}/multi_core>
)

# Include NS local interface headers
target_include_directories(tfm_api_ns
    PUBLIC
        ${NS_INTERFACE_DIR}
        ${NSID_MGR_DIR}
)

# Include selection of Secure Partitions from TF-M build.
# It can be replaced by NS side configurations later.
target_link_libraries(tfm_api_ns
    PUBLIC
        tfm_config
)

target_compile_definitions(tfm_api_ns
    PUBLIC
        $<$<STREQUAL:${TEST_PSA_API},IPC>:PSA_API_TEST_IPC>
        $<$<BOOL:${TFM_NS_MANAGE_NSID}>:TFM_NS_MANAGE_NSID>
        $<$<BOOL:${TEST_NS_MANAGE_NSID}>:TEST_NS_MANAGE_NSID>
        $<$<BOOL:${CONFIG_TFM_USE_TRUSTZONE}>:CONFIG_TFM_USE_TRUSTZONE>
        $<$<BOOL:${TFM_MULTI_CORE_TOPOLOGY}>:TFM_MULTI_CORE_TOPOLOGY>
        $<$<BOOL:${TFM_PARTITION_NS_AGENT_MAILBOX}>:TFM_PARTITION_NS_AGENT_MAILBOX>
        $<$<BOOL:${TFM_MULTI_CORE_NS_OS}>:TFM_MULTI_CORE_NS_OS>
        $<$<AND:$<BOOL:${TFM_MULTI_CORE_NS_OS_MAILBOX_THREAD}>,$<BOOL:${TFM_MULTI_CORE_NS_OS}>>:TFM_MULTI_CORE_NS_OS_MAILBOX_THREAD>
)

set(COMPILER_NS_FLAGS ${COMPILER_CP_FLAG}   -mbranch-protection=bti+pac-ret)
#-mbranch-protection=bti+pac-ret --stack-protector

target_compile_options(tfm_api_ns
    PUBLIC
        ${COMPILER_NS_FLAGS}
)

target_sources(tfm_api_ns
    PRIVATE
        $<$<BOOL:${TFM_NS_MANAGE_NSID}>:${NSID_MGR_DIR}/tfm_nsid_manager.c>
)

target_sources(tfm_api_ns PRIVATE
    $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${INTERFACE_SRC_DIR}/tfm_platform_api.c>
    $<$<BOOL:${TFM_PARTITION_PROTECTED_STORAGE}>:${INTERFACE_SRC_DIR}/tfm_ps_api.c>
    $<$<BOOL:${TFM_PARTITION_INTERNAL_TRUSTED_STORAGE}>:${INTERFACE_SRC_DIR}/tfm_its_api.c>
    $<$<BOOL:${TFM_PARTITION_CRYPTO}>:${INTERFACE_SRC_DIR}/tfm_crypto_api.c>
    $<$<BOOL:${TFM_PARTITION_INITIAL_ATTESTATION}>:${INTERFACE_SRC_DIR}/tfm_attest_api.c>
    $<$<BOOL:${TFM_PARTITION_FIRMWARE_UPDATE}>:${INTERFACE_SRC_DIR}/tfm_fwu_api.c>
    )

if (TFM_PARTITION_NS_AGENT_MAILBOX)
    target_sources(tfm_api_ns PRIVATE
        ${INTERFACE_SRC_DIR}/multi_core/tfm_multi_core_ns_api.c
        ${INTERFACE_SRC_DIR}/multi_core/tfm_multi_core_psa_ns_api.c
        $<$<NOT:$<BOOL:${TFM_MULTI_CORE_NS_OS_MAILBOX_THREAD}>>:${INTERFACE_SRC_DIR}/multi_core/tfm_ns_mailbox.c>
        $<$<AND:$<BOOL:${TFM_MULTI_CORE_NS_OS}>,$<BOOL:${TFM_MULTI_CORE_NS_OS_MAILBOX_THREAD}>>:${INTERFACE_SRC_DIR}/multi_core/tfm_ns_mailbox_thread.c>
    )

    # NS RTOS specific implementation of NS mailbox
    target_sources(tfm_api_ns PRIVATE
        $<$<BOOL:${TFM_MULTI_CORE_NS_OS}>:${NS_INTERFACE_DIR}/multi_core/tfm_ns_mailbox_rtos_api.c>
        $<$<BOOL:${TEST_NS_MULTI_CORE}>:${NS_INTERFACE_DIR}/multi_core/tfm_ns_mailbox_test.c>
    )
endif()

target_sources(tfm_api_ns PRIVATE
    $<$<BOOL:${CONFIG_TFM_USE_TRUSTZONE}>:${INTERFACE_SRC_DIR}/tfm_psa_ns_api.c>
    # NS specific implementation of NS interface dispatcher
    $<$<BOOL:${CONFIG_TFM_USE_TRUSTZONE}>:${INTERFACE_SRC_DIR}/os_wrapper/tfm_ns_interface_rtos.c>
)

target_link_libraries(tfm_api_ns
    PRIVATE
        $<$<BOOL:${CONFIG_TFM_USE_TRUSTZONE}>:tfm_s_veneers>
        platform_ns
)

############################# TFM NS app #######################################

add_executable(tfm_ns)

target_sources(tfm_ns
    PRIVATE
        main_ns.c
        test_app.c
        test_app.h
        $<$<BOOL:${CONFIG_GNU_SYSCALL_STUB_ENABLED}>:${CMAKE_SOURCE_DIR}/platform/ext/common/syscalls_stub.c>
)

target_link_libraries(tfm_ns
    PRIVATE
        CMSIS_5_tfm_ns
#        tfm_test_app
        tfm_log
        platform_ns
        $<$<BOOL:${CONFIG_TFM_ENALBE_PROFILING}>:tfm_ns_profiling>
        attestation_ns
        #pacbti_service_api_ns
        #tfm_t_cose_common
        #tfm_t_cose_defs
        #tfm_qcbor_ns
        #tfm_api_ns
)

set_target_properties(tfm_ns PROPERTIES
    SUFFIX ".axf"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_options(tfm_ns
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:-Wl,-z,noexecstack,-Map=${CMAKE_BINARY_DIR}/bin/tfm_ns.map>
        $<$<C_COMPILER_ID:ARMClang>:--map>
        $<$<C_COMPILER_ID:IAR>:--map\;${CMAKE_BINARY_DIR}/bin/tfm_ns.map>
)

add_convert_to_bin_target(tfm_ns)

############################# CMSIS ############################################

include(FetchContent)

set(FETCHCONTENT_QUIET FALSE)
cmake_policy(SET CMP0079 NEW)

add_library(CMSIS_5_tfm_ns INTERFACE)

target_sources(CMSIS_5_tfm_ns
    INTERFACE
        ${NSID_MGR_DIR}/tz_shim_layer.c
        ${CMSIS_5_PATH}/RTOS2/RTX/Config/RTX_Config.c
        ${CMSIS_5_PATH}/RTOS2/RTX/Source/rtx_lib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/os_wrapper_cmsis_rtos_v2.c
        $<$<BOOL:${TFM_NS_MANAGE_NSID}>:${CMAKE_CURRENT_SOURCE_DIR}/tfm_nsid_map_table.c>
)

target_include_directories(CMSIS_5_tfm_ns
    INTERFACE
        ${CMSIS_5_PATH}/Core/Include
        ${CMSIS_5_PATH}/RTOS2/Include
        ${CMSIS_5_PATH}/RTOS2/RTX/Include
        ${CMSIS_5_PATH}/RTOS2/RTX/Config
        $<$<BOOL:${TFM_NS_MANAGE_NSID}>:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(CMSIS_5_tfm_ns
    INTERFACE
        platform_ns
)

target_compile_definitions(CMSIS_5_tfm_ns
    INTERFACE
        $<$<BOOL:${TFM_NS_MANAGE_NSID}>:TFM_NS_MANAGE_NSID>
)

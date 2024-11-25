/*
 * Copyright (c) 2017-2022, Arm Limited. All rights reserved.
 * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
//#include <core_cm85.h>
#include <arm_acle.h>
#include "cmsis_os2.h"
#include "cmsis_compiler.h"
//#include "tfm_ns_interface.h"
//#include "tfm_nsid_manager.h"
#include "test_app.h"
//#include "tfm_plat_ns.h"
#include <pac_armv81.h>
#include "driver/Driver_USART.h"
#include "device_cfg.h"

//#include "tfm_multi_core_api.h"

#include "tfm_log.h"
#include "uart_stdout.h"
#if (CONFIG_TFM_FLOAT_ABI >= 1)
#include "cmsis.h"
#endif


#include "stdio.h"
#include "stdlib.h"

#define TFM_DEFAULT_NSID        ((int32_t)-1)

/**
 * \brief Platform peripherals and devices initialization.
 *        Can be overridden for platform specific initialization.
 *
 * \return  ARM_DRIVER_OK if the initialization succeeds
 */
__WEAK int32_t tfm_ns_platform_init(void)
{
    stdio_init();

    return ARM_DRIVER_OK;
}

/**
 * \brief Platform peripherals and devices de-initialization.
 *        Can be overridden for platform specific initialization.
 *
 * \return  ARM_DRIVER_OK if the de-initialization succeeds
 */
__WEAK int32_t tfm_ns_platform_uninit(void)
{
    stdio_uninit();

    return ARM_DRIVER_OK;
}


__WEAK int32_t tfm_ns_cp_init(void)
{
#if (CONFIG_TFM_FLOAT_ABI >= 1)
#ifdef __GNUC__
    /* Enable NSPE privileged and unprivilged access to the FP Extension */
    SCB->CPACR |= (3U << 10U*2U)     /* enable CP10 full access */
                  | (3U << 11U*2U);  /* enable CP11 full access */
#endif
#endif
    return ARM_DRIVER_OK;
}


/**
 * \brief main() function
 */
#ifndef __GNUC__
__attribute__((noreturn))
#endif

extern uint32_t tfm_ns_interface_init(void);


int main(void)
{   

    if (tfm_ns_platform_init() != ARM_DRIVER_OK) {
        /* Avoid undefined behavior if platform init failed */
        while(1);
    }

    if (tfm_ns_cp_init() != ARM_DRIVER_OK) {
        /* Avoid undefined behavior if co-porcessor init failed */
        while(1);
    }

    (void) osKernelInitialize();

    if(tfm_ns_interface_init() != 0) {
        LOG_MSG("Error initializing\n");
    }

    static osThreadFunc_t thread_func = test_app;

    static const osThreadAttr_t thread_attr = {
        .name = "test_thread",
        .stack_size = 4096U,
        .tz_module = ((TZ_ModuleId_t)TFM_DEFAULT_NSID)
    };

    (void) osThreadNew(thread_func, NULL, &thread_attr);

    LOG_MSG("Non-Secure system starting...\r\n");
    if (osKernelGetState() == osKernelReady)
        (void) osKernelStart();


    /* Reached only in case of error */
    for (;;) {
    }
  
   

}

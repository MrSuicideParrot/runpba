/*
 * Copyright (c) 2017-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test_app.h"
#include "tfm_log.h"

#include "attestation.h"
//#include "tfm_pacbti_service.h"

#include "uart_stdout.h"

#define ATTEST_TOKEN_MAX_SIZE  0x250

char* super_secret_flag = "my-super-secret";

void echo_processing(int size) {
    int message_length = size;
    int array_length = size;
    
    if (array_length > 40) {
    array_length = 40;
    }
    char buffer[array_length];
    int result = stdio_input_string(buffer, message_length);
    buffer[message_length] = '\0';
    printf("Message received  %d !\r\nProcessing\r\n", result);
}

void printf_processing(int size) {
    int message_length = size;
    int array_length = size;
    
    if (array_length > 40) {
    array_length = 40;
    }
    char buffer[array_length];
    int result = stdio_input_string(buffer, message_length);
    buffer[message_length] = '\0';
    printf("Message received  %d !\r\nProcessing\r\n", result);

    printf("\r\n");
    printf("Feito\r\n");
}

/**
 * \brief Services test thread
 *
 */
__attribute__((noreturn))
void test_app(void *argument)
{


     __asm volatile(
        // CONTROL register: set UPAC_EN to enable PAC in privileged mode.
       //"BKPT\n\t"
       "mrs    r2,CONTROL\n\t"
       "orr    r2,r2,#0x00000040\n\t"
       "msr    CONTROL,r2\n\t"
        
        // CONTROL register: set UBTI_EN, to enable BTI in privileged mode.
       "mrs    r2,CONTROL\n\t"
       //"orr    r2,r2,#0x00000010\n\t"
       //"orr    r2,r2,#0x00000000\n\t"
       "msr    CONTROL,r2\n\t"
    );

        char cenas[40]; // = (char *)malloc(sizeof(char)*8);
    int returned;

UNUSED_VARIABLE(argument);
LOG_MSG("Thread starting!\r\n");

Q_USEFUL_BUF_MAKE_STACK_UB(token_storage, ATTEST_TOKEN_MAX_SIZE);
struct q_useful_buf_c completed_token;
struct q_useful_buf_c tmp;
uint32_t token_encode_options = 0;

tmp = (struct q_useful_buf_c) { (uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 64 };


int_fast16_t return_value = token_main_alt(token_encode_options,
                                tmp,
                                token_storage,
                                &completed_token);

if(return_value == 0) {
    LOG_MSG("Success!\n");
} else {
    LOG_MSG("Failed to receive token! - %d \n", return_value);
}

while (1)
{
    uint8_t header[2];
    stdio_input_string(header, 2);

    switch (header[0])
    {
    case 1:
        LOG_MSG("ECHO with length - %d.\r\n", header[1]);
        //printf_processing(header[1]);
        echo_processing(header[1]);
        break;
    
    default:
        LOG_MSG("Unknown %d\r\n", header[0]);
        break;
    }

    LOG_MSG("HERE WE GO AGAIN\r\n");
}

    /* Output EOT char for test environments like FVP. */
    LOG_MSG("\x04");

    /* End of test */
    for (;;) {
    }
}

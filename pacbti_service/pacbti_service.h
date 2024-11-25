
#ifndef __PACBTI_SERVICE_H__
#define __PACBTI_SERVICE_H__

#include <stdint.h>

typedef struct __attribute__((packed)) ContextStateFrame {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t return_address;
    uint32_t xpsr;
} sContextStateFrame;

#define PACBTI_STORAGE_UID_MACRO 75

uint8_t get_runtime_status_of_nspe();

uint8_t get_fault_status();

psa_status_t get_fault(sContextStateFrame *frame);


#endif /* __PACBTI_SERVICE_H__ */
#ifndef __TFM_PACBTI_SERVICE_H__
#define __TFM_PACBTI_SERVICE_H__

#include "psa/error.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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


psa_status_t tfm_runpba_get_runtime_status_of_nspe(uint8_t *runtime_status);
psa_status_t tfm_runpba_get_fault_status(uint8_t *fault_status);
psa_status_t tfm_runpba_get_fault(sContextStateFrame *frame);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PACBTI_SERVICE_H__ */
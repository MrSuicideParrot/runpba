#include "tfm_device_dirty.h"
#include "tfm_attest_hal.h"
#include "tfm_plat_otp.h"
#include "tfm_sp_log.h"
#include "psa/error.h"
#include <string.h>

#include "pacbti_service.h"

#include "psa/internal_trusted_storage.h"

#define U_PAC_ENABLED 0x8u
#define P_PAC_ENABLED 0x4u
#define U_BTI_ENABLED 0x2u
#define P_BTI_ENABLED 0x1u

uint8_t get_runtime_status_of_nspe(){
    uint32_t control_ns;
    uint8_t status;
    __asm volatile (
        "mrs %[buf],CONTROL_NS"
        : [buf] "=r" (control_ns)
    );

    /*
        The control_ns uses
 
        U PAC 0x80
        P PAC 0x40
        U BTI 0x20
        P BTI 0x10
    
    */

    memcpy(&status, &control_ns, sizeof(uint8_t));
    
    status = (status & 0xFF)  >> 4;

    return status;
}



uint8_t get_fault_status() {

    struct psa_storage_info_t info = {0};
    psa_storage_uid_t storage_uid = PACBTI_STORAGE_UID_MACRO;

    psa_status_t status = psa_its_get_info(storage_uid, &info);

    switch (status) {
        case PSA_ERROR_DOES_NOT_EXIST:
            return 0;
        case PSA_SUCCESS:
            return 1;
        default:
            // Undefined error
            return 2;
    }
}


psa_status_t get_fault(sContextStateFrame *frame) {
    size_t frame_size = 0;
    psa_storage_uid_t storage_uid = PACBTI_STORAGE_UID_MACRO;

    psa_status_t status = psa_its_get(storage_uid, 0,  sizeof(sContextStateFrame), (void*)frame, &frame_size);
  
    return status;
}
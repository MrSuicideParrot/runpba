
#include "tfm_pacbti_service_def.h"
#include "tfm_pacbti_service.h"
#include "psa/client.h"
#include "psa_manifest/sid.h"

#include <stdio.h>

psa_status_t tfm_runpba_get_runtime_status_of_nspe(uint8_t *runtime_status) {
    psa_status_t status;
    
    psa_invec in_vec[] = {
        {NULL, 0}
    };

    psa_outvec out_vec[] = {
        {runtime_status, sizeof(uint8_t)}
    };

    status = psa_call(TFM_PACBTI_SERVICE_HANDLE,
                    PACBTI_SERVICE_GET_NSPE_STATUS,
                    in_vec,  IOVEC_LEN(in_vec),
                    out_vec, IOVEC_LEN(out_vec));

    return status;
}

psa_status_t tfm_runpba_get_fault_status(uint8_t *fault_status) {
    psa_status_t status;

    psa_invec in_vec[] = {
        {NULL, 0}
    };

    psa_outvec out_vec[] = {
        {.base =fault_status, .len = sizeof(uint8_t)}
    };


    status = psa_call(TFM_PACBTI_SERVICE_HANDLE,
                    PACBTI_SERVICE_GET_FAULT_STATUS,
                    in_vec,  IOVEC_LEN(in_vec),
                    out_vec, IOVEC_LEN(out_vec));

    return status;
}

psa_status_t tfm_runpba_get_fault(sContextStateFrame *frame) {
    psa_status_t status;

    psa_invec in_vec[] = {
        {NULL, 0}
    };

    psa_outvec out_vec[] = {
        {frame, sizeof(sContextStateFrame)}
    };


    status = psa_call(TFM_PACBTI_SERVICE_HANDLE,
                    PACBTI_SERVICE_GET_FAULT,
                    in_vec,  IOVEC_LEN(in_vec),
                    out_vec, IOVEC_LEN(out_vec));

    return status;
}


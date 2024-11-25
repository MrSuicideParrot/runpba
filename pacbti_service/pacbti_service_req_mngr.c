#include "psa/service.h"
#include "psa/error.h"
#include "pacbti_service.h"
#include "tfm_sp_log.h"
#include "interface/include/psa/internal_trusted_storage.h"
#include "interface/include/tfm_its_defs.h"
#include "tfm_hal_defs.h"
#include "cmsis.h"
#include "interrupt.h"
#include "load/interrupt_defs.h"
#include "psa_manifest/tfm_pacbti_service.h"
#include "pacbti_service.h"
#include "tfm_pacbti_service_def.h"

//TFM_SECUREFAULT_IRQ

#define ATTEST_TOKEN_MAX_SIZE  0x250

static struct irq_t pacbti_irq = {0};


void GPIO0_9_IRQ_Handler(void){
    spm_handle_interrupt(pacbti_irq.p_pt, pacbti_irq.p_ildi);
}


void GPIO0_9_Handler(void){
    spm_handle_interrupt(pacbti_irq.p_pt, pacbti_irq.p_ildi);
}


static sContextStateFrame lastFrame = {0};

void pacbti_setContextFrame(sContextStateFrame *frame) {
    memcpy(&lastFrame, frame, sizeof(sContextStateFrame));
}


//enum tfm_hal_status_t irq_43_init(void *p_pt, const struct irq_load_info_t *p_ildi) {
enum tfm_hal_status_t gpio0_9_irq_init(void *p_pt, const struct irq_load_info_t *p_ildi) {
    
    pacbti_irq.p_pt = p_pt;
    pacbti_irq.p_ildi = p_ildi;


    /* Set MHU interrupt priority to the same as PendSV (the lowest)
    * TODO: Consider advantages/disadvantages of setting it one higher
    */
    NVIC_SetPriority(GPIO0_9_IRQn, NVIC_GetPriority(PendSV_IRQn));
  
    /* CMU_MHU1 is a secure peripheral, so its IRQs have to target S state */
    NVIC_ClearTargetState(GPIO0_9_IRQn);
    
    return TFM_HAL_SUCCESS;
}

/*

psa_flih_result_t usage_fault_signal_flih(){
    
    psa_storage_uid_t storage_uid = 75;
    psa_status_t status;

    uint8_t write_data = 1;
    //size_t read_data_len;

    status = psa_its_set(storage_uid, sizeof(write_data), (void*)&write_data, PSA_STORAGE_FLAG_NONE);


    //status = psa_its_remove(storage_uid);
    if (status != PSA_SUCCESS) {
        LOG_DBGFMT("FALHOU REMOVE");
    } else {
        LOG_DBGFMT("Não falhou");
    }

    return 0;
}


psa_flih_result_t usage_fault_signal_slih(){
    
    psa_storage_uid_t storage_uid = 75;
    psa_status_t status;

    uint8_t write_data = 1;
    //size_t read_data_len;

    status = psa_its_set(storage_uid, sizeof(write_data), (void*)&write_data, PSA_STORAGE_FLAG_NONE);


    //status = psa_its_remove(storage_uid);
    if (status != PSA_SUCCESS) {
        LOG_DBGFMT("FALHOU REMOVE");
    } else {
        LOG_DBGFMT("Não falhou");
    }

    return 0;
}
*/



void handle_pacbtifault(void) {

    psa_storage_uid_t storage_uid = PACBTI_STORAGE_UID_MACRO; // WHY???
    psa_status_t status;

    status = psa_its_set(storage_uid, sizeof(lastFrame), (void*)&lastFrame, PSA_STORAGE_FLAG_NONE);
    if (status != PSA_SUCCESS) {
        LOG_ERRFMT("Failed setting into storage");
    } else {
        LOG_INFFMT("PACBTI fault triggered\n");
        LOG_INFFMT("###### FAULT CONTEXT ######\n");
        LOG_INFFMT("$pc:    %p\n", lastFrame.return_address);
        LOG_INFFMT("$r0:    %p\n", lastFrame.r0);
        LOG_INFFMT("$r1:    %p\n", lastFrame.r1);
        LOG_INFFMT("$r2:    %p\n", lastFrame.r2);
        LOG_INFFMT("$r3:    %p\n", lastFrame.r3);
        LOG_INFFMT("$r12:   %p\n", lastFrame.r12);
        LOG_INFFMT("$lr:    %p\n", lastFrame.lr);
        LOG_INFFMT("######\n");


        LOG_INFFMT("Fault status: %p\n", get_fault_status());
        LOG_INFFMT("PACBTI runtime status: %p\n", get_runtime_status_of_nspe());
    }

    LOG_INFFMT("Rebooting device\n");
    tfm_core_panic(); // Probably reboot the
    //tfm_hal_system_reset(); // if we want to reset the device

}

psa_status_t handle_fault_status_signal(const psa_msg_t *msg) {
    uint8_t fault_status = get_fault_status();

    fault_status = fault_status << 4;

    psa_write(msg->handle, 0, &fault_status, sizeof(fault_status));

    return PSA_SUCCESS;
}

psa_status_t handle_nspe_status_signal(const psa_msg_t *msg) {
    uint8_t nspe_status = get_runtime_status_of_nspe();

    psa_write(msg->handle, 0, &nspe_status, sizeof(nspe_status));
    
    return PSA_SUCCESS;
}

psa_status_t handle_get_fault_signal(const psa_msg_t *msg) {
    psa_status_t status;

    sContextStateFrame frame = {0};

    status = get_fault(&frame);

    if(status == PSA_SUCCESS) {
        psa_write(msg->handle, 0, &frame, sizeof(sContextStateFrame));
        return PSA_SUCCESS;
    }
    // TODO CONTINUE
    return PSA_SUCCESS;

}

void handle_service_signal(){
    psa_msg_t msg;
    psa_status_t status;

    status = psa_get(TFM_PACBTI_SERVICE_SIGNAL, &msg);
    if (status != PSA_SUCCESS) {
        return;
    }

    switch (msg.type) {
        case PACBTI_SERVICE_GET_FAULT_STATUS:
            handle_fault_status_signal(&msg);
            psa_reply(msg.handle, status);
            break;

        case PACBTI_SERVICE_GET_NSPE_STATUS:
            handle_nspe_status_signal(&msg);
            psa_reply(msg.handle, status);
            break;

        case PACBTI_SERVICE_GET_FAULT:
            handle_get_fault_signal(&msg);
            psa_reply(msg.handle, status);
            break;

        default:
            status = PSA_ERROR_PROGRAMMER_ERROR;
            psa_reply(msg.handle, status);
    }
}

void tfm_pacbti_entry(void)
{
    psa_signal_t signals = 0;

    /* Secure Partition initialization */
    //example_init();

    psa_irq_enable(RUNPBAFAULT_SIGNAL);

    /*
     * Continually wait for one or more of the partition's RoT Service or
     * interrupt signals to be asserted and then handle the asserted
     * signal(s).
     */
    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        LOG_INFFMT("\nSignal reiceved\n");
        if (signals & RUNPBAFAULT_SIGNAL) {
            handle_pacbtifault();
        } else if (signals & TFM_PACBTI_SERVICE_SIGNAL ) {
            handle_service_signal();
        
        } else{
            /* Should not come here */
            __asm("bkpt 4");
        }
    }
}

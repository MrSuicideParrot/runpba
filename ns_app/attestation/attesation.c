#include "psa/initial_attestation.h"
#include "attestation.h"
#include "tfm_log.h"
//#include "attest_token_decode.h"
//#include "psa/crypto.h"

static void dump_token(struct q_useful_buf_c *token)
{
    int i;
    unsigned char num;

    LOG_MSG("\r\n");
    LOG_MSG("########### token(len: %d): ###########\r\n", token->len);
    for (i = 0; i < token->len; ++i) {
        num = ((unsigned char *)token->ptr)[i];
        LOG_MSG(" 0x%X%X", (num & 0xF0) >> 4, num & 0x0F);
        if (((i + 1) % 8) == 0) {
            LOG_MSG("\r\n");
        }
    }
    LOG_MSG("\r\n############## token end  ##############\r\n");
}

int token_main_alt(uint32_t option_flags,
                   struct q_useful_buf_c nonce,
                   struct q_useful_buf buffer,
                   struct q_useful_buf_c *completed_token)
{
    psa_status_t                 return_value;
    size_t                       token_buf_size;
    size_t                       completed_token_size;
    struct q_useful_buf_c        actual_nonce;
    Q_USEFUL_BUF_MAKE_STACK_UB(  actual_nonce_storage, 64);

    if(nonce.len == 64 && (q_useful_buf_is_value(nonce, 0) == SIZE_MAX)) {
        /* Go into special option-packed nonce mode */
        actual_nonce = q_useful_buf_copy(actual_nonce_storage, nonce);
        /* Use memcpy as it always works and avoids type punning */
        memcpy((uint8_t *)actual_nonce_storage.ptr,
               &option_flags,
               sizeof(uint32_t));
    } else {
        actual_nonce = nonce;
    }
     
    token_buf_size = buffer.len;
    return_value = psa_initial_attest_get_token(actual_nonce.ptr,
                                                actual_nonce.len,
                                                buffer.ptr,
                                                token_buf_size,
                                                &completed_token_size);
    *completed_token =
        (struct q_useful_buf_c){buffer.ptr, completed_token_size};
    if (return_value != PSA_SUCCESS) {
        return (int)return_value;
    }

    dump_token(completed_token);

    return 0;
}



#ifndef AMP_amp_return_codes_H
#define AMP_amp_return_codes_H

#include <errno.h>



#if defined(__cplusplus)
extern "C" {
#endif

    
    
    enum amp_return_code {
        amp_success_return_code = 0,
        amp_nomem_return_code = ENOMEM,
        amp_busy_return_code = EBUSY,
        amp_timeout_return_code = ETIMEDOUT,
        amp_unsupported_return_code = ENOSYS,
        amp_error_return_code = 666
    };
    
    typedef enum amp_return_code amp_return_code_t;
    
    
#define AMP_SUCCESS (amp_success_return_code)
#define AMP_NOMEM (amp_nomem_return_code)
#define AMP_BUSY (amp_busy_return_code)
#define AMP_TIMEOUT (amp_timeout_return_code)
#define AMP_UNSUPPORTED (amp_unsupported_return_code)
#define AMP_ERROR (amp_error_return_code)
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif
        

#endif /* AMP_amp_return_codes_H */

/*
 * Copyright (c) 2009-2010, Bjoern Knafla
 * http://www.bjoernknafla.com/
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are 
 * met:
 *
 *   * Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the Bjoern Knafla 
 *     Parallelization + AI + Gamedev Consulting nor the names of its 
 *     contributors may be used to endorse or promote products derived from 
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *
 * Definition of all return codes that amp functions might return.
 *
 * Always use the symbols and not the actual values associated with the 
 * symbols as these might change.
 */

#ifndef AMP_amp_return_codes_H
#define AMP_amp_return_codes_H

#include <errno.h>



#if defined(__cplusplus)
extern "C" {
#endif

    
    /**
     * Return codes used by amp.
     */
    enum amp_return_code {
        amp_success_return_code = 0, /**< Operation successful */
        amp_nomem_return_code = ENOMEM, /**< Not enough memory */
        amp_busy_return_code = EBUSY, /**< Resource in use by other thread */
        amp_unsupported_return_code = ENOSYS, /**< Operation not supported by backend */
        amp_timeout_return_code, /**< Waited on busy resource till timeout */
        amp_error_return_code = 666 /**< Another error occured */
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

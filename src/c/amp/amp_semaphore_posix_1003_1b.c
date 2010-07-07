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
 * Shallow wrapper around POSIX 1003 1b semaphores.
 *
 * TODO: @todo Test implementation on a platform supporting POSIX 1003 1b 
 *             semaphores. Until then: take care.
 */

#include "amp_semaphore.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include <unistd.h>

#include "amp_stddef.h"
#include "amp_return_code.h"
#include "amp_raw_semaphore.h"



#if !defined(AMP_USE_POSIX_1003_1B_SEMAPHORES)
#   error Build configuration problem - this source file shouldn't be compiled.
#endif


#if !defined(_POSIX_SEMAPHORES)
#   error POSIX 1003.1b-1993 semaphores aren't supported.
#endif



int amp_raw_semaphore_init(amp_semaphore_t semaphore,
                           amp_semaphore_counter_t init_count)
{
    assert(NULL != semaphore);
    assert((amp_semaphore_counter_t)0 <= init_count);
    assert(AMP_RAW_SEMAPHORE_COUNT_MAX >= (amp_raw_semaphore_counter_t)init_count);
    
    if ((amp_semaphore_counter_t)0 > init_count
        || AMP_RAW_SEMAPHORE_COUNT_MAX < (amp_raw_semaphore_counter_t)init_count) {
        
        return AMP_ERROR;
    }
    
    errno = 0;
    int const retval = sem_init(&semaphore->semaphore, 
                                0, /* Don't share semaphore between processes */
                                (unsigned int)init_count);
    int return_code = AMP_SUCCESS;
    if (0 != retval) {
        switch (errno) {
            case EINVAL: /* init_count exceeds SEM_VALUE_MAX */
                /* Fallthrough */
            case ENOSPC: /* Resources exhausted or trying to create more than SEM_NSEMS_MAX semaphore */
                return_code = AMP_ERROR;
                break;
            case ENOSYS:
                assert(0); /* Use another amp semaphore backend */
                return_code = AMP_UNSUPPORTED;
                break;
            default: /* EPERM - programming error */
                assert(0);
                return_code = AMP_ERROR;
        }
    }
    
    return return_code;
}



int amp_raw_semaphore_finalize(amp_semaphore_t semaphore)
{
    assert(NULL != semaphore);
    
    errno = 0;
    int retval = sem_destroy(&semaphore->semaphore);
    int return_code = AMP_SUCCESS;
    if(0 != retval) {
        switch (errno) {
            case ENOSYS:
                assert(0); /* Use another amp semaphore backend */
                return_code = AMP_UNSUPPORTED;
                break;
            default: /* EINVAL, EBUSY - programming error */
                assert(0);
                return_code = AMP_ERROR;
        }
    }

    return return_code;
}



int amp_semaphore_wait(amp_semaphore_t semaphore)
{
    assert(NULL != semaphore);
    
    errno = 0;
    int retval = sem_wait(&semaphore->semaphore);
    int return_code = AMP_SUCCESS;
    if(0 != retval) {        
        switch (errno) {
            case ENOSYS:
                assert(0); /* Use another amp semaphore backend */
                return_code = AMP_UNSUPPORTED;
                break;
            case EINTR: /* sem_wait interrupted by a signal */
                return_code = AMP_ERROR;
                break;
            default: /* EINVAL, EDEADLK - programming error */
                assert(0);
                return_code = AMP_ERROR;
        }
    }
    
    return return_code;
}



int amp_semaphore_signal(amp_semaphore_t semaphore)
{
    assert(NULL != semaphore);
    
    errno = 0;
    int retval = sem_post(&semaphore->semaphore);    
    int return_code = AMP_SUCCESS;
    if(0 != retval) {        
        switch (errno) {
            case ENOSYS:
                assert(0); /* Use another amp semaphore backend */
                return_code = AMP_UNSUPPORTED;
                break;
            default:
                assert(0); /* Programming error */
                return_code = AMP_ERROR;
        }
    }
    
    return return_code;    
}




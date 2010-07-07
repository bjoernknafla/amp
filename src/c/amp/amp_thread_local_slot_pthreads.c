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
 * Shallow wrapper around Pthreads thread-specific data.
 */



#include "amp_raw_thread_local_slot.h"


#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include "amp_stddef.h"
#include "amp_return_code.h"



int amp_raw_thread_local_slot_init(amp_thread_local_slot_key_t key)
{
    assert(NULL != key);
    
    int retval = pthread_key_create(&(key->key), NULL);
    switch (retval) {
        case 0:
            /* retval already equals AMP_SUCCESS */
            break;
        case ENOMEM:
            /* retval already equals AMP_NOMEM */
            break;
        case EAGAIN:
            retval = AMP_ERROR;
            break;
        default:
            assert(0); /* Programming error */
            retval = AMP_ERROR;
    }
    
    return retval;
}



int amp_raw_thread_local_slot_finalize(amp_thread_local_slot_key_t key)
{
    int retval = pthread_key_delete(key->key);
    if (0 != retval) {
        assert(0); /* Programming error */
        retval = AMP_ERROR;
    }
    
    return retval;
}



int amp_thread_local_slot_set_value(amp_thread_local_slot_key_t key,
                                    void *value)
{
    int retval = pthread_setspecific(key->key, value);
    switch (retval) {
        case 0:
            /* retval already equals AMP_SUCCESS */
            break;
        case ENOMEM:
            /* retval already equals AMP_NOMEM */
            break;
        default: /* EINVAL - programming error */
            assert(0);
            retval = AMP_ERROR;
    }
    
    return retval;
}



void* amp_thread_local_slot_value(amp_thread_local_slot_key_t key)
{
    /**
     * TODO: @todo Add a debug status flag to check if a key has been 
     * initialized correctly.
     */
    return pthread_getspecific(key->key);
}



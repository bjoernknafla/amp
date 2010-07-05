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
 * Cross platform functionality shared by all platform backends.
 * Look into the different amp_raw_thread_local_slot_  backend source files for
 * the platform / backend specific implementations.
 */


#include "amp_thread_local_slot.h"

#include <assert.h>
#include <stddef.h>

#include "amp_stddef.h"
#include "amp_return_code.h"
#include "amp_raw_thread_local_slot.h"



int amp_thread_local_slot_create(amp_thread_local_slot_key_t* key,
                                 amp_allocator_t allocator)
{
    amp_thread_local_slot_key_t tmp_key = AMP_THREAD_LOCAL_SLOT_UNINITIALIZED;
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != key);
    assert(NULL != allocator);
    
    *key = AMP_THREAD_LOCAL_SLOT_UNINITIALIZED;
    
    tmp_key = (amp_thread_local_slot_key_t)AMP_ALLOC(allocator,
                                                     sizeof(*tmp_key));
    if (NULL == tmp_key) {
        return AMP_NOMEM;
    }
    
    retval = amp_raw_thread_local_slot_init(tmp_key);
    if (AMP_SUCCESS == retval) {
        *key = tmp_key;
    } else {
        int const rc = AMP_DEALLOC(allocator,
                                   tmp_key);
        assert(AMP_SUCCESS == rc);
        (void)rc;
    }
    
    return retval;
}



int amp_thread_local_slot_destroy(amp_thread_local_slot_key_t* key,
                                  amp_allocator_t allocator)
{
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != key);
    assert(NULL != *key);
    assert(NULL != allocator);
    
    retval = amp_raw_thread_local_slot_finalize(*key);
    if (AMP_SUCCESS == retval) {
        retval = AMP_DEALLOC(allocator,
                             *key);
        assert(AMP_SUCCESS == retval);
        if (AMP_SUCCESS == retval) {
            *key = AMP_THREAD_LOCAL_SLOT_UNINITIALIZED;
        }
    }
    
    return retval;
}



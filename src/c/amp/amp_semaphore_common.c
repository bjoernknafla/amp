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
 * Implementation shared by all amp semaphore backends.
 */

#include "amp_semaphore.h"
#include "amp_raw_semaphore.h"

#include <assert.h>

#include "amp_stddef.h"
#include "amp_return_code.h"



int amp_semaphore_create(amp_semaphore_t* semaphore,
                         amp_allocator_t allocator,
                         amp_semaphore_counter_t init_count)
{
    amp_semaphore_t tmp_sema = AMP_SEMAPHORE_UNINITIALIZED;
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != semaphore);
    assert((amp_semaphore_counter_t)0 <= init_count);
    assert(AMP_RAW_SEMAPHORE_COUNT_MAX >= (amp_raw_semaphore_counter_t)init_count);
    assert(NULL != allocator);
    
    
    *semaphore = AMP_SEMAPHORE_UNINITIALIZED;
    
    if ((amp_semaphore_counter_t)0 > init_count
        || AMP_RAW_SEMAPHORE_COUNT_MAX < (amp_raw_semaphore_counter_t)init_count) {
        
        return AMP_ERROR;
    }

    tmp_sema = (amp_semaphore_t)AMP_ALLOC(allocator,
                                           sizeof(*tmp_sema));
    if (NULL == tmp_sema) {
        return AMP_NOMEM;
    }
    
    retval = amp_raw_semaphore_init(tmp_sema,
                                    init_count);
    if (AMP_SUCCESS == retval) {
        *semaphore = tmp_sema;
    } else {
        int const rc = AMP_DEALLOC(allocator, tmp_sema);
        assert(AMP_SUCCESS == rc);
        (void)rc;
    }
    
    return retval;
}



int amp_semaphore_destroy(amp_semaphore_t* semaphore,
                          amp_allocator_t allocator)
{
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != semaphore);
    assert(NULL != *semaphore);
    assert(NULL != allocator);
    
    retval = amp_raw_semaphore_finalize(*semaphore);
    if (AMP_SUCCESS == retval) {
        retval = AMP_DEALLOC(allocator,
                             *semaphore);
        assert(AMP_SUCCESS == retval);
        if (AMP_SUCCESS == retval) {
            *semaphore = AMP_SEMAPHORE_UNINITIALIZED;
        }
    }
    
    return retval;
}



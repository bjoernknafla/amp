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
 * Implementation shared by all amp mutex backends.
 */

#include "amp_mutex.h"

#include <assert.h>
#include <stddef.h>

#include "amp_return_code.h"
#include "amp_raw_mutex.h"



int amp_mutex_create(amp_mutex_t* mutex,
                     amp_allocator_t allocator)
{
    amp_mutex_t tmp_mutex = AMP_MUTEX_UNINITIALIZED;
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != mutex);
    assert(NULL != allocator);
    
    tmp_mutex = (amp_mutex_t)AMP_ALLOC(allocator,
                                       sizeof(*tmp_mutex));
    if (NULL == tmp_mutex) {
        return AMP_NOMEM;
    }
 
    retval = amp_raw_mutex_init(tmp_mutex);
    if (AMP_SUCCESS == retval) {
        *mutex = tmp_mutex;
    } else {
        int const rc = AMP_DEALLOC(allocator,
                                   tmp_mutex);
        assert(AMP_SUCCESS == rc);
        (void)rc;
    }
    
    return retval;
}



int amp_mutex_destroy(amp_mutex_t* mutex,
                      amp_allocator_t allocator)
{
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != mutex);
    assert(NULL != *mutex);
    assert(NULL != allocator);
    
    retval = amp_raw_mutex_finalize(*mutex);
    if (AMP_SUCCESS == retval) {
        retval = AMP_DEALLOC(allocator,
                             *mutex);
        assert(AMP_SUCCESS == retval);
        if (AMP_SUCCESS == retval) {
            *mutex = AMP_MUTEX_UNINITIALIZED;
        }
    }
    
    return retval;
}



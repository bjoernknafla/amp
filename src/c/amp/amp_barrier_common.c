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
  * Common implementation of amp_barrier shared between all backends.
  */

#include "amp_raw_barrier.h"

#include <assert.h>

#include "amp_stddef.h"
#include "amp_return_code.h"



int amp_barrier_create(amp_barrier_t* barrier,
                       amp_allocator_t allocator,
                       amp_barrier_count_t init_count)
{
    amp_barrier_t tmp_barrier = AMP_BARRIER_UNINITIALIZED;
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != barrier);
    assert(0 < init_count);
    assert(NULL != allocator);

    if (0 >= init_count) {
        
        return AMP_ERROR;
    }
    
    *barrier = AMP_BARRIER_UNINITIALIZED;
    
    tmp_barrier = (amp_barrier_t)AMP_ALLOC(allocator,
                                           sizeof(*tmp_barrier));
    if (NULL == tmp_barrier) {
        return AMP_NOMEM;
    }
    
    retval = amp_raw_barrier_init(tmp_barrier, init_count);
    if (AMP_SUCCESS == retval) {
        *barrier = tmp_barrier;
    } else {
        int const rv = AMP_DEALLOC(allocator, tmp_barrier);
        assert(AMP_SUCCESS == rv);
        (void)rv;
    }

    return retval;
}



int amp_barrier_destroy(amp_barrier_t* barrier,
                        amp_allocator_t allocator)
{
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != barrier);
    assert(NULL != *barrier);
    assert(NULL != allocator);
    
    
    retval = amp_raw_barrier_finalize(*barrier);
    if (AMP_SUCCESS == retval) {
        retval = AMP_DEALLOC(allocator, *barrier);
        assert(AMP_SUCCESS == retval);
        if (AMP_SUCCESS == retval) {
            *barrier = AMP_BARRIER_UNINITIALIZED;
        }
    }
    
    return retval;
}




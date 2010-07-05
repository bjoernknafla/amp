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
 * Shallow wrapper around Mac OS X 10.6 libdispatch semaphores.
 */

#include "amp_semaphore.h"

#include <assert.h>
#include <stddef.h>

#include "amp_stddef.h"
#include "amp_return_code.h"
#include "amp_raw_semaphore.h"



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
    
    /**
     * TODO: @todo Use the create func with an init count the moment Apple fixes
     *             the sem creation with an init count so that waiting on it and 
     *             then calling release won't crash if no number of signals 
     *             matching the waits have been called somewhere (needn't be 
     *             before the wait but before the release).
     */
    /* sem->semaphore = dispatch_semaphore_create(init_count); */
    semaphore->semaphore = dispatch_semaphore_create(0l);

    if (NULL == semaphore->semaphore) {
        return AMP_NOMEM;
    }
    
    /**
     * TODO: @todo Remove the for loop the moment Apple fixes the sem creation
     *             with an init count so that waiting on it and then calling
     *             release won't crash if no number of signals matching
     *             the waits have been called somewhere (needn't be before the 
     *             wait but before the release).
     */
    for (amp_semaphore_counter_t i = 0; i < init_count; ++i) {
        long const signal_retval = dispatch_semaphore_signal(semaphore->semaphore);
        assert(0 == signal_retval && "During creation no thread must wait on the semaphore.");
        (void)signal_retval;
    }
    
    return AMP_SUCCESS;
}



int amp_raw_semaphore_finalize(amp_semaphore_t semaphore)
{
    assert(NULL != semaphore);
    assert(NULL != semaphore->semaphore);

    dispatch_release(semaphore->semaphore);
    
    return AMP_SUCCESS;
}



int amp_semaphore_wait(amp_semaphore_t semaphore)
{
    assert(NULL != semaphore);
    assert(NULL != semaphore->semaphore);

    long retval = dispatch_semaphore_wait(semaphore->semaphore, DISPATCH_TIME_FOREVER);
    
    assert(0 == retval && "Timeout should not occur when waiting for DISPATCH_TIME_FOREVER.");
    (void)retval;
    
    return AMP_SUCCESS;
}



int amp_semaphore_signal(amp_semaphore_t semaphore)
{
    assert(NULL != semaphore);
    assert(NULL != semaphore->semaphore);
    
    (void)dispatch_semaphore_signal(semaphore->semaphore);
    
    return AMP_SUCCESS;
}


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

#include "amp_raw_semaphore.h"



#include <errno.h>
#include <assert.h>



#include "amp_stddef.h"



int amp_raw_semaphore_init(struct amp_raw_semaphore_s *sem,
                           amp_raw_semaphore_count_t init_count)
{
    /* No way to reliably detect if the sem is already initialized... */
    
    assert(NULL != sem);
    assert(0l <= init_count);
    
    if (NULL == sem) {
        return EINVAL;
    }
    
    if (0l > init_count) {
        return EINVAL;
    }
    
    /**
     * TODO: @todo Use the create func with an init count the moment Apple fixes
     *             the sem creation with an init count so that waiting on it and 
     *             then calling release won't crash if no number of signals 
     *             matching the waits have been called somewhere (needn't be 
     *             before the wait but before the release).
     */
    /* sem->semaphore = dispatch_semaphore_create(init_count); */
    sem->semaphore = dispatch_semaphore_create(0l);

    if (NULL == sem->semaphore) {
        return ENOMEM;
    }
    
    /**
     * TODO: @todo Remove the for loop the moment Apple fixes the sem creation
     *             with an init count so that waiting on it and then calling
     *             release won't crash if no number of signals matching
     *             the waits have been called somewhere (needn't be before the 
     *             wait but before the release).
     */
    for (long i = 0; i < init_count; ++i) {
        long const signal_retval = dispatch_semaphore_signal(sem->semaphore);
        (void) signal_retval;
    }
    
    
    return AMP_SUCCESS;
}



int amp_raw_semaphore_finalize(struct amp_raw_semaphore_s *sem)
{
    assert(NULL != sem);
    assert(NULL != sem->semaphore);
    
    if (NULL == sem)  {
        return EINVAL;
    }
    
    if (NULL == sem->semaphore) {
        return EINVAL;
    }
    
    dispatch_release(sem->semaphore);
    
    return AMP_SUCCESS;
}



int amp_raw_semaphore_wait(struct amp_raw_semaphore_s *sem)
{
    assert(NULL != sem);
    assert(NULL != sem->semaphore);

    long retval = dispatch_semaphore_wait(sem->semaphore, DISPATCH_TIME_FOREVER);
    (void)retval;
    
    assert(0 == retval && "Unexpected error.");
    
    return AMP_SUCCESS;
}



int amp_raw_semaphore_signal(struct amp_raw_semaphore_s *sem)
{
    assert(NULL != sem);
    assert(NULL != sem->semaphore);
    
    dispatch_semaphore_signal(sem->semaphore);
    
    return AMP_SUCCESS;
}


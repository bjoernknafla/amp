/*
 * Copyright (c) 2009, Bjoern Knafla
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
 * Shallow wrapper around Windows semaphores.
 *
 * TODO: @todo Check for all Windows event waiting code if internal 
 *             event pumping events should be handled or not during waiting.
 */



#include "amp_raw_semaphore.h"

/* Include NULL */
#include <stddef.h>

/* Include assert */
#include <assert.h>

/* Include errno, EINVAL, ENOSYS, ENOSPC, EPERM, EBUSY, EINTR, EDEADLK */
#include <errno.h>


/* Include AMP_SUCCESS */
#include "amp_stddef.h"






int amp_raw_semaphore_init(struct amp_raw_semaphore_s *sem,
                           amp_semaphore_count_t init_count)
{
    assert(NULL != sem);
    

}



int amp_raw_semaphore_finalize(struct amp_raw_semaphore_s *sem)
{
    assert(NULL != sem);
    

}



int amp_raw_semaphore_wait(struct amp_raw_semaphore_s *sem)
{
    assert(NULL != sem);
    

}


int amp_raw_semaphore_signal(struct amp_raw_semaphore_s *sem)
{
    assert(NULL != sem);
    

}



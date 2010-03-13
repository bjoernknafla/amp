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
 * Implementation of POSIX thread condition variable alikes using actual POSIX
 * threads condition variables.
 */

#include "amp_raw_condition_variable.h"



#include <errno.h>
#include <assert.h>



#include "amp_raw_mutex.h"



int amp_raw_condition_variable_init(amp_raw_condition_variable_t cond)
{
    assert(NULL != cond);
    
    if (NULL == cond) {
        return EINVAL;
    }
    
    int retval = pthread_cond_init(&cond->cond, NULL);
    assert(EINVAL != retval && "Condition variable attribute is invalid.");
    assert(EBUSY != retval && "Condition variable is already initialized.");
    
    assert( (0 == retval || EAGAIN == retval || ENOMEM == retval) 
           && "Unexpected error.");
    
    return retval;
}



int amp_raw_condition_variable_finalize(amp_raw_condition_variable_t cond)
{
    assert(NULL != cond);
    
    if ( NULL == cond) {
        return EINVAL;
    }
    
    int retval = pthread_cond_destroy(&cond->cond);
    assert(EBUSY != retval && "Condition variable is in use.");
    assert(EINVAL != retval && "Condition variable is invalid.");
    
    assert(0 == retval && "Unexpected error.");
    
    return retval;
}



int amp_raw_condition_variable_broadcast(amp_raw_condition_variable_t cond)
{
    assert(NULL != cond);
    
    int retval = pthread_cond_broadcast(&cond->cond);
    assert(EINVAL != retval && "Condition variable is invlaid.");
    
    assert(0 == retval && "Unexpected error.");
    
    return retval;
}



int amp_raw_condition_variable_signal(amp_raw_condition_variable_t cond)
{
    assert(NULL != cond);
    
    int retval = pthread_cond_signal(&cond->cond);
    assert(EINVAL != retval && "Condition variable is invalid.");
    
    assert(0 == retval && "Unexpected error.");
    
    return retval;
}



int amp_raw_condition_variable_wait(amp_raw_condition_variable_t cond,
                                    amp_raw_mutex_t mutex)
{
    assert(NULL != cond);
    assert(NULL != mutex);

    int retval = pthread_cond_wait(&cond->cond, &mutex->mutex);

    assert(EINVAL != retval && "Condition variable or mutex is invalid or different mutexes for concurrent waits or mutex is not owned by calling thread.");

    assert(0 == retval && "Unexpected error.");
    
    return retval;
}



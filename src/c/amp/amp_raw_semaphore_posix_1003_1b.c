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
 * Shallow wrapper around POSIX 1003 1b semaphores.
 */

#error Implementation is completely untested and haven't been compiled yet. Take care!


#include "amp_raw_semaphore.h"

/* Include NULL */
#include <stddef.h>

/* Include assert */
#include <assert.h>

/* Include errno, EINVAL, ENOSYS, ENOSPC, EPERM, EBUSY, EINTR, EDEADLK */
#include <errno.h>

/* Include _POSIX_SEMAPHORES (if supported) */
#include <unistd.h>

/* Include AMP_SUCCESS */
#include "amp_stddef.h"



#if !defined(AMP_USE_POSIX_1003_1B_SEMAPHORES)
#   error Build configuration problem - this source file shouldn't be compiled.
#endif


#if !defined(_POSIX_SEMAPHORES)
#   error POSIX 1003.1b-1993 semaphores aren't supported.
#endif



int amp_raw_semaphore_init(struct amp_raw_semaphore_s *sem,
                                     amp_semaphore_count_t init_count)
{
    assert(NULL != sem);
    /* TODO: @todo Decide if to assert or no to assert. */
    assert( ((amp_raw_semaphore_count_t)0 <= init_count 
             && AMP_RAW_SEMAPHORE_COUNT_MAX >= init_count) 
           && "init_count must be greater or equal to zero and lesser or equal to AMP_RAW_SEMAPHORE_COUNT_MAX.");
    
    int const retval = sem_init(&sem->semaphore, 
                                0, /* Don't share semaphore between processes */
                                (unsigned int)init_count);
    assert(EPERM != errno && "Process lacks privileges.");
    assert( (0 == retval || ENOSYS == errno || EINVAL == errno) 
           && "Unexpected error.");
    
    int return_code = AMP_SUCCESS;
    if (0 != retval) {
        return_code = errno;
    }
    
    return return_code;
}



int amp_raw_semaphore_finalize(struct amp_raw_semaphore_s *sem)
{
    assert(NULL != sem);
    
    int retval = sem_destroy(&sem->semaphore);
    assert(EBUSY != errno && "Threads are still blocked on the semaphore.");
    assert((0 == retval || ENOSYS == errno)&& "Unexpected error.");
    
    int return_code = AMP_SUCCESS;
    if(0 != retval) {
        return_code = errno;
    }

    return return_code;
}



int amp_raw_semaphore_wait(struct amp_raw_semaphore_s *sem)
{
    assert(NULL != sem);
    
    int retval = sem_wait(&sem->semaphore);
    assert(EINVAL != errno && "sem->semaphore is not a valid semaphore.");
    assert(EDEADLK != errno && "A deadlock was detected.");
    assert( (0 == retval
             || EINTR == errno 
             || ENOSYS == errno) 
           && "Unexpected error.");
    
    int return_code = AMP_SUCCESS;
    if(0 != retval) {        
        return_code = errno;
    }
    
    return return_code;
}


int amp_raw_semaphore_signal(struct amp_raw_semaphore_s *sem)
{
    assert(NULL != sem);
    
    int retval = sem_post(&sem->semaphore);
    assert(EINVAL != errno && "sem->semaphore is not a valid semaphore.");
    assert((0 == retval || ENOSYS == errno || EOVERFLOW == errno) 
           && "Unexpected error.");
    
    int return_code = AMP_SUCCESS;
    if(0 != retval) {        
        return_code = errno;
    }
    
    return return_code;    
}




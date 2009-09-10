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
 * TODO: @todo Look through all error return codes and identify which 
 *             are purely internal (error in this code) and shouldn't leave the
 *             function but be fixed.
 */


#include "amp_raw_semaphore.h"

// Include EAGAIN, ENOMEM, EPERM, EBUSY, EINVAL
#include <errno.h>

// Include assert
#include <assert.h>

// Include AMP_SUCCESS
#include "amp_stddef.h"



#if !defined(AMP_USE_PTHREADS) || defined(AMP_USE_POSIX_1003_1B_SEMAPHORES)
#   error Build configuration problem - this source file shouldn't be compiled.
#endif



int amp_raw_semaphore_init(struct amp_raw_semaphore_s *sem,
                                     amp_raw_semaphore_count_t init_count)
{
    /* TODO: @todo only assert or only check for error and return error code? */
    if (!(NULL != sem) ||
        !((amp_raw_semaphore_count_t)0 <= init_count) ||
        !(AMP_RAW_SEMAPHORE_COUNT_MAX >= init_count)) {
        
        assert(NULL != sem);
        assert((amp_raw_semaphore_count_t)0 <= init_count);
        assert(AMP_RAW_SEMAPHORE_COUNT_MAX >= init_count);
        
        return EINVAL;
    }
    
    int retval = pthread_mutex_init(&sem->mutex, NULL);
    if (0 != retval) {
        assert(EAGAIN != retval && "Insufficient system resources.");
        assert(ENOMEM != retval && "Insufficient memory.");
        assert(EPERM != retval && "No privileges to perform operation.");
        assert(EBUSY != retval && "Mutex already initialized.");
        assert(EINVAL != retval && "Attribute is invalid.");
        assert(0 == retval && "Unknown error code.");
        
        return retval;
    }
    
    retval = pthread_cond_init(&sem->a_thread_can_pass, NULL);
    if (0 != retval) {
        assert(EAGAIN != retval && "Insufficient system resources.");
        assert(ENOMEM != retval && "Insufficient memory.");
        assert(EBUSY != retval && "Condition variable is already initialized.");
        assert(EINVAL != retval && "Attribute is invalid.");
        assert(0 == retval && "Unknown error code.");
        
        int const retv = pthread_mutex_destroy(&sem->mutex);
        
        if (0 != retv) {
            assert(EBUSY != retv && "Mutex is in use.");
            assert(EINVAL != retv && "Mutex is invalid.");
            assert(0 == retval && "Unknown error code.");
            
            return retv;
        }
        
        return retval;
    }
    
    sem->count = init_count;
    
    return AMP_SUCCESS;
}


int amp_raw_semaphore_finalize(struct amp_raw_semaphore_s *sem)
{
    /* TODO: @todo only assert or only check for error and return error code? */
    if (!(NULL != sem)) {
        assert(NULL != sem);
        
        return EINVAL;
    }
    
    
    int retval = pthread_cond_destroy(&sem->a_thread_can_pass);
    if (0 != retval) {
        assert(EBUSY != retval && "Condition variable is in use.");
        assert(EINVAL != retval && "Condition varialbe is invalid.");
        assert(0 == retval && "Unknown error code.");
        
        return retval;
    }
    
    retval = pthread_mutex_destroy(&sem->mutex);
    if (0 != retval) {
        assert(EBUSY != retval && "Mutex is in use.");
        assert(EINVAL != retval && "Mutex is invalid.");
        assert(0 == retval && "Unknown error code.");
        
        return retval;
    }
    
    return AMP_SUCCESS;
}


int amp_raw_semaphore_wait(struct amp_raw_semaphore_s *sem)
{    
    if( !(NULL != sem)) {
        assert(NULL != sem);
        
        return EINVAL;
    }
    
    int retval = pthread_mutex_lock(&sem->mutex);
    if (0 != retval) {
        assert(EINVAL != retval && "Mutex is invalid or thread priority exceeds mutex priority ceiling.");
        assert(EDEADLK != retval && "Calling thread already owns the mutex.");
        assert(0 == retval && "Unknown error code.");
        
        return retval;
    }
    
    {
        while (0 == sem->count) {
            int const rv = pthread_cond_wait(&sem->a_thread_can_pass, &sem->mutex);
            if (0 != rv) {
                assert(EINVAL != rv && "Condition variable or mutex is invalid or different mutexes are used for concurrent waits or mutex is not owned by calling thread.");
                assert(0 == rv && "Unknown error code.");
                
                int const r = pthread_mutex_unlock(&sem->mutex);
                if (0 != r) {
                    assert(EINVAL != r && "Mutex is invalid.");
                    assert(EPERM != r && "Calling thread does not own mutex.");
                    assert(0 == r && "Unknown error code.");
                    
                    // TODO: @todo How to handle this kind of dependent errors?
                    return r;
                }
                
                return rv;
            }
        }
        
        --(sem->count);
    }
    retval = pthread_mutex_unlock(&sem->mutex);
    if (0 != retval) {
        assert(EINVAL != retval && "Mutex is invalid.");
        assert(EPERM != retval && "Calling thread does not own mutex.");
        assert(0 == retval && "Unknown error code.");
        
        return retval;
    }
    
    
    
    
    return AMP_SUCCESS;
}


int amp_raw_semaphore_signal(struct amp_raw_semaphore_s *sem)
{
    if (!(NULL != sem)){
        assert(NULL != sem);
        
        return EINVAL;
    }
    
    int retval = pthread_mutex_lock(&sem->mutex);
    if (0 != retval) {
        assert(EINVAL != retval && "Mutex is invalid or thread priority exceeds mutex priority ceiling.");
        assert(EDEADLK != retval && "Calling thread already owns the mutex.");
        assert(0 == retval && "Unknown error code.");
        
        return retval;
    }
    {
        ++(sem->count);
        
        // Could also signal after unlocking the mutex. Needs experimentation.
        int const rv = pthread_cond_signal(&sem->a_thread_can_pass);
        if (0 != rv) {
            assert(EINVAL != rv && "Condition variable is invalid.");
            assert(0 == rv && "Unknown error code.");
            
            int const r = pthread_mutex_unlock(&sem->mutex);
            if (0 != r) {
                assert(EINVAL != r && "Mutex is invalid.");
                assert(EPERM != r && "Calling thread does not own mutex.");
                assert(0 == r && "Unknown error code.");
                
                // TODO: @todo How to handle this kind of dependent errors?
                return r;
            }
            
            
            return rv;
        }
    }
    retval = pthread_mutex_unlock(&sem->mutex);
    if (0 != retval) {
        assert(EINVAL != retval && "Mutex is invalid.");
        assert(EPERM != retval && "Calling thread does not own mutex.");
        assert(0 == retval && "Unknown error code.");
        
        return retval;
    }
    
    return AMP_SUCCESS;
}



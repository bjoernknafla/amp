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
 * Shallow wrapper around Pthread mutex to emulate an amp raw semaphore.
 *
 * TODO: @todo Look through all error return codes and identify which 
 *             are purely internal (error in this code) and shouldn't leave the
 *             function but be fixed.
 */


#include "amp_semaphore.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include "amp_stddef.h"
#include "amp_raw_semaphore.h"



#if !defined(AMP_USE_PTHREADS) || defined(AMP_USE_POSIX_1003_1B_SEMAPHORES)
#   error Build configuration problem - this source file shouldn't be compiled.
#endif



int amp_raw_semaphore_init(amp_semaphore_t semaphore,
                           amp_semaphore_counter_t init_count)
{
    assert(NULL != semaphore);
    assert((amp_semaphore_counter_t)0 <= init_count);
    assert(AMP_RAW_SEMAPHORE_COUNT_MAX >= (amp_raw_semaphore_counter_t)init_count);
    
    if (NULL == semaphore
        || (amp_semaphore_counter_t)0 > init_count
        || AMP_RAW_SEMAPHORE_COUNT_MAX < (amp_raw_semaphore_counter_t)init_count) {
        
        /* Set to zero to potentially show detectable strange behavior
         * if the non-initialized mutex doesn't crash or act up and the dev
         * doesn't check and react to the return code.
         */
        semaphore->count = 0;
        
        return EINVAL;
    }
    
    semaphore->count = init_count;
    
    int retval = AMP_SUCCESS;
    pthread_mutexattr_t mutex_attributes;
    /* If the system lacks memory can return ENOMEM. */
    retval = pthread_mutexattr_init(&mutex_attributes);
    assert((0 == retval || ENOMEM == retval) && "Unexpected error.");
    if (0 != retval) {
        return retval;
    }
    
    /* Use an error checking mutex while asserts/debug mode are enabled. */
#if !defined(NDEBUG)
    retval = pthread_mutexattr_settype(&mutex_attributes, 
                                       PTHREAD_MUTEX_ERRORCHECK);
    assert(EINVAL != retval && "Attribute or type invalid.");
    assert(0 == retval && "Unexpected error.");
#endif /* !defined(NDEBUG) */
    
    /*
     * Might generate EAGAIN or ENOMEM errors which are handed back to the 
     * caller.
     * EINVAL, EPERM, and EBUSY  error codes are implementation problems and are
     * therefore checked while debugging.
     */ 
    retval = pthread_mutex_init(&semaphore->mutex,&mutex_attributes);
    assert(EINVAL != retval && "Attribute is invalid.");
    assert(EPERM != retval && "No privileges to perform operation.");
    assert(EBUSY != retval && "Mutex already initialized.");
    
    /* 
     *Get rid of the mutex attribute - it isn't used anymore (nor in
     * mutex creation error case, nor in non-error case).
     */
    int const mattr_destroy_retval = pthread_mutexattr_destroy(&mutex_attributes);
    assert(EINVAL != mattr_destroy_retval && "Mutex attributes invalid.");
    assert(0 == mattr_destroy_retval && "Unexpected error.");
    assert( (0 == retval
             || AMP_SUCCESS == retval
             || EAGAIN == retval 
             || ENOMEM == retval )
           && "Unexpected error.");
    /* 
     *If an error occured return error code from mutex creation after mutex
     * attributes have been destroyed.
     */
    if (0 != retval) {
        return retval;
    }
    
    /*
     * Create a process private condition variable.
     * Lack of resources and memory errors are reported back as EAGAIN, ENOMEM. 
     * Other errors indicate programming errors and trigger assertions in debug 
     * mode (and are also returned in non-debug mode).
     */
    retval = pthread_cond_init(&semaphore->a_thread_can_pass, NULL);
    assert(EBUSY != retval && "Condition variable is already initialized.");
    assert(EINVAL != retval && "Attribute is invalid.");
    assert( (0 == retval
             || AMP_SUCCESS == retval
             || EAGAIN == retval 
             || ENOMEM == retval )
           && "Unexpected error.");
    
    if (0 != retval) {
        int const retv = pthread_mutex_destroy(&semaphore->mutex);
        assert(EBUSY != retv && "Mutex is in use.");
        assert(EINVAL != retv && "Mutex is invalid.");
        assert( (0 == retv
                 || AMP_SUCCESS == retval
                 || EBUSY == retval 
                 || EINVAL == retval )
               && "Unexpected error.");
        if (0 != retv ) {
            return retv;
        }
        
        
        return retval;
    }
        
    return AMP_SUCCESS;
}


int amp_raw_semaphore_finalize(amp_semaphore_t semaphore)
{
    assert(NULL != semaphore);
    
    int retval = pthread_cond_destroy(&semaphore->a_thread_can_pass);
    assert(EBUSY != retval && "Condition variable is in use.");
    assert(EINVAL != retval && "Condition varialbe is invalid.");
    assert(0 == retval && "Unexpected error.");
    
    if (0 != retval) {
        return retval;
    }
    
    retval = pthread_mutex_destroy(&semaphore->mutex);
    assert(EBUSY != retval && "Mutex is in use.");
    assert(EINVAL != retval && "Mutex is invalid.");
    assert(0 == retval && "Unexpected error.");
    
    if (0 != retval) {
        return retval;
    }
    
    return AMP_SUCCESS;
}


int amp_semaphore_wait(amp_semaphore_t semaphore)
{    
    assert(NULL != semaphore);
    
    int retval = AMP_SUCCESS;
    int const mlock_retval = pthread_mutex_lock(&semaphore->mutex);
    assert(EINVAL != mlock_retval && "Mutex is invalid or thread priority exceeds mutex priority ceiling.");
    assert(EDEADLK != mlock_retval && "Calling thread already owns the mutex.");
    assert(0 == mlock_retval && "Unknown error code.");
    
    if (0 != mlock_retval) {

        return mlock_retval;
    }
    
    {
        while ((0 == semaphore->count) && (AMP_SUCCESS == retval)) {
            /* The following asserts trigger on user programming errors. */
            int const rv = pthread_cond_wait(&semaphore->a_thread_can_pass, 
                                             &semaphore->mutex);
            assert(EINVAL != rv && "Condition variable or mutex is invalid or different mutexes are used for concurrent waits or mutex is not owned by calling thread.");
            assert(0 == rv && "Unexpected error.");
            
            if (0 != rv) {
                retval = rv;
            }
        }
        
        if (AMP_SUCCESS == retval) {
            --(semaphore->count);
        }
    }
    int const munlock_retval = pthread_mutex_unlock(&semaphore->mutex);
    assert(EINVAL != munlock_retval && "Mutex is invalid.");
    assert(EPERM != munlock_retval && "Calling thread does not own mutex.");
    assert(0 == munlock_retval && "Unexpected error.");
    if (0 != munlock_retval) {
        return munlock_retval;
    }
    
    assert(AMP_SUCCESS == retval && "Unexpected error.");
    
    return retval;
}


int amp_semaphore_signal(amp_semaphore_t semaphore)
{
    assert(NULL != semaphore);
    
    int retval = AMP_SUCCESS;
    
    int const mlock_retval = pthread_mutex_lock(&semaphore->mutex);
    assert(EINVAL != mlock_retval && "Mutex is invalid or thread priority exceeds mutex priority ceiling.");
    assert(EDEADLK != mlock_retval && "Calling thread already owns the mutex.");
    assert(0 == mlock_retval && "Unexpected error.");
    
    if (0 != mlock_retval) {
        /* retval = mlock_retval; */
        return mlock_retval;
    }
    {
        if (semaphore->count < (amp_semaphore_counter_t)AMP_RAW_SEMAPHORE_COUNT_MAX) {
            ++(semaphore->count); 
            
            /* Could also signal after unlocking the mutex. Needs experimentation.*/
            int const rv = pthread_cond_signal(&semaphore->a_thread_can_pass);
            assert(EINVAL != rv && "Condition variable is invalid.");
            assert(0 == rv && "Unexpected error.");
            
            if (0 != rv) {
                retval = rv;
            }
            
        } else {
            retval = EOVERFLOW;
        }
    }
    int const munlock_retval = pthread_mutex_unlock(&semaphore->mutex);
    assert(EINVAL != munlock_retval && "Mutex is invalid.");
    assert(EPERM != munlock_retval && "Calling thread does not own mutex.");
    assert(0 == munlock_retval && "Unexpected error.");
    if (0 != munlock_retval) {
        /* retval = munlock_retval; */
        return munlock_retval;
    }
    
    assert( (AMP_SUCCESS == retval || EOVERFLOW == retval) 
           && "Unexpected error.");
    
    return retval;
}



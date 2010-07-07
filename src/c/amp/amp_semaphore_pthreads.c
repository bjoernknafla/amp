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
 */


#include "amp_semaphore.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include "amp_stddef.h"
#include "amp_return_code.h"
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
    
    if ((amp_semaphore_counter_t)0 > init_count
        || AMP_RAW_SEMAPHORE_COUNT_MAX < (amp_raw_semaphore_counter_t)init_count) {
        
        /* Set to zero to potentially show detectable strange behavior
         * if the non-initialized mutex doesn't crash or act up and the dev
         * doesn't check and react to the return code.
         */
        semaphore->count = 0;
        
        return AMP_ERROR;
    }
    
    semaphore->count = init_count;
    
    pthread_mutexattr_t mutex_attributes;
    int retval = pthread_mutexattr_init(&mutex_attributes);
    if (0 != retval) {
        switch (retval) {
            case ENOMEM:
                /* retval is already equal to AMP_NOMEM */
                break;
            default:
                assert(0); /* Programming error */
                retval = AMP_ERROR;
        }
        
        return retval;
    }
    
    /* Use an error checking mutex while asserts/debug mode are enabled. */
#if !defined(NDEBUG)
    retval = pthread_mutexattr_settype(&mutex_attributes, 
                                       PTHREAD_MUTEX_ERRORCHECK);
    assert(0 == retval);
#endif /* !defined(NDEBUG) */
    
    retval = pthread_mutex_init(&semaphore->mutex,&mutex_attributes);
    assert(0 == retval || EAGAIN == retval || ENOMEM == retval);
    
    int const mattr_destroy_retval = pthread_mutexattr_destroy(&mutex_attributes);
    assert(0 == mattr_destroy_retval);
    (void)mattr_destroy_retval;
    
    /* 
     *If an error occured return error code from mutex creation after mutex
     * attributes have been destroyed.
     */
    if (0 != retval) {
        switch (retval) {
            case ENOMEM:
                /* retval is already equal to AMP_NOMEM */
                break;
            case EAGAIN:
                retval = AMP_ERROR;
                break;
            default: /* EPERM, EBUSY, EINVAL - programming error */
                assert(0);
                retval = AMP_ERROR;
        }
        
        return retval;
    }
    
    retval = pthread_cond_init(&semaphore->a_thread_can_pass, NULL);
    if (0 != retval) {
        switch (retval) {
            case ENOMEM:
                /* retval is already equal to AMP_NOMEM */
                break;
            case EAGAIN:
                retval = AMP_ERROR;
                break;
            default: /* EBUSY, EINVAL - programming error */
                assert(0);
                retval = AMP_ERROR;
        }
        
        int const retv = pthread_mutex_destroy(&semaphore->mutex);
        assert(0 == retv); /* Programming error */
        (void)retv;
        
        return retval;
    }
    
    return AMP_SUCCESS;
}


int amp_raw_semaphore_finalize(amp_semaphore_t semaphore)
{
    assert(NULL != semaphore);
    
    int const retval0 = pthread_cond_destroy(&semaphore->a_thread_can_pass);
    int const retval1 = pthread_mutex_destroy(&semaphore->mutex);
    if (0 != retval0 || 0 != retval1) {
        assert(0); /* Programming error */
        return AMP_ERROR;
    }
    
    return AMP_SUCCESS;
}


int amp_semaphore_wait(amp_semaphore_t semaphore)
{    
    assert(NULL != semaphore);
    
    int retval = AMP_SUCCESS;
    int const mlock_retval = pthread_mutex_lock(&semaphore->mutex);
    if (0 != mlock_retval) {
        assert(0); /* Programming error */
        return AMP_ERROR;
    }
    {
        while ((0 >= semaphore->count) && (AMP_SUCCESS == retval)) {
            retval = pthread_cond_wait(&semaphore->a_thread_can_pass, 
                                             &semaphore->mutex);
            assert(0 == retval); /* Programming error */
        }
        
        if (AMP_SUCCESS == retval) {
            --(semaphore->count);
        } else {
            retval = AMP_ERROR;
        }
    }
    int const munlock_retval = pthread_mutex_unlock(&semaphore->mutex);
    assert(0 == munlock_retval);
    (void)munlock_retval;
    
    return retval;
}


int amp_semaphore_signal(amp_semaphore_t semaphore)
{
    assert(NULL != semaphore);
    
    int retval = AMP_SUCCESS;
    
    int const mlock_retval = pthread_mutex_lock(&semaphore->mutex);
    if (0 != mlock_retval) {
        assert(0); /* Programming error */
        return AMP_ERROR;
    }
    {
        if (semaphore->count < (amp_semaphore_counter_t)AMP_RAW_SEMAPHORE_COUNT_MAX) {
            ++(semaphore->count); 
            
            /* Could also signal after unlocking the mutex. Needs experimentation.*/
            retval = pthread_cond_signal(&semaphore->a_thread_can_pass);
            if (0 != retval) {
                assert(0); /* Programming error */
                retval = AMP_ERROR;
            }
            
        } else {
            assert(0); /* Programming error */
            retval = AMP_ERROR;
        }
    }
    int const munlock_retval = pthread_mutex_unlock(&semaphore->mutex);
    assert(0 == munlock_retval);
    (void)munlock_retval;
    
    return retval;
}



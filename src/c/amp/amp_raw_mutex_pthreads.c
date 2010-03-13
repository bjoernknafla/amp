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
 * Shallow raw wrapper around the Pthread mutex primitive.
 */


#include "amp_raw_mutex.h"



#include <assert.h>
#include <errno.h>


#include "amp_stddef.h"



int amp_raw_mutex_init(amp_raw_mutex_t mutex)
{
    assert(NULL != mutex);
    
    int retval = AMP_SUCCESS;
    
    pthread_mutexattr_t mutex_attributes;
    retval = pthread_mutexattr_init(&mutex_attributes);
    if (0 != retval) {
        /* assert(ENOMEN != retval && "Insufficient memory for mutex attributes."); */
        return retval;
    }
    
    /* Use an error checking mutex while asserts/debug mode are enabled. */
#if !defined(NDEBUG)
    retval = pthread_mutexattr_settype(&mutex_attributes, 
                                       PTHREAD_MUTEX_ERRORCHECK);
    assert(EINVAL != retval && "Attribute or type invalid.");
    assert(0 == retval && "Unexpected error.");
#endif
    
    /* Might generate EAGAIN or ENOMEM errors which are handed back to the 
     * caller.
     * EINVAL, EPERM, and EBUSY error codes are implementation problems and are
     * therefore checked while debugging.
     */ 
    retval = pthread_mutex_init(&mutex->mutex, &mutex_attributes);
    assert(EINVAL != retval  && "Attribute is invalid.");
    assert(EPERM != retval && "No privileges to perform operation.");
    assert(EBUSY != retval && "Mutex already initialized.");   
    
    /* Get rid of the mutex attribute - it isn't used anymore. */
    int const mattr_destroy_retval = pthread_mutexattr_destroy(&mutex_attributes);
    assert(EINVAL != mattr_destroy_retval && "Mutex attributes invalid.");

    assert( (0 == retval
             || AMP_SUCCESS == retval
             || EAGAIN == retval 
             || ENOMEM == retval )
           && "Unexpected error.");
    
    return retval;
}



int amp_raw_mutex_finalize(amp_raw_mutex_t mutex)
{
    assert(NULL != mutex);
    
    int retval = AMP_SUCCESS;
    retval = pthread_mutex_destroy(&mutex->mutex);
    assert(EINVAL != retval && "Mutex is invalid.");
    assert(EBUSY != retval && "Mutex is in use.");
    
    assert( (AMP_SUCCESS == retval || 0 == retval ) && "Unexpected error.");
    
    return retval;
}



int amp_raw_mutex_lock(amp_raw_mutex_t mutex)
{
    assert(NULL != mutex);
    
    int retval = AMP_SUCCESS;
    /* All non-zero error codes returned indicate implementation errors and are
     * therefore checked while debugging.
     */
    retval = pthread_mutex_lock(&mutex->mutex);
    assert(EINVAL != retval && "Thread priority exceeds mutex priority ceiling or mutex is invalid.");
    assert(EDEADLK != retval && "Calling thread already owns mutex.");
    
    assert( (AMP_SUCCESS == retval || 0 == retval ) && "Unexpected error.");
    
    return retval;
}



int amp_raw_mutex_trylock(amp_raw_mutex_t mutex)
{
    assert(NULL != mutex);
    
    int retval = AMP_SUCCESS;
    
    /* Returns 0 on success or EBUSY if can't lock - these values are returned
     * to the caller. Other errors that might be returned, e.g. EINVAL or 
     * EDEADLK are implementation errors and therefore checked while debugging.
     */
    retval = pthread_mutex_trylock(&mutex->mutex);
    assert(EINVAL != retval
           && "Thread priority exceeds mutex priority ceiling or mutex is invalid.");
    assert(EDEADLK != retval
           && "Calling thread already owns mutex.");
    
    assert( (0 == retval 
             || AMP_SUCCESS == retval 
             || EBUSY == retval)
           && "Unexpected error.");
    
    return retval;
}



int amp_raw_mutex_unlock(amp_raw_mutex_t mutex)
{
    assert(NULL != mutex);
    
    int retval = AMP_SUCCESS;
    retval = pthread_mutex_unlock(&(mutex->mutex));
    
    assert(EINVAL != retval && "Mutex is invalid.");
    assert(EPERM != retval && "Calling thread does not own mutex.");
    
    assert( (AMP_SUCCESS == retval || 0 == retval ) && "Unexpected error.");
    
    return retval;
}



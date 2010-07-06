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


#include "amp_mutex.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include "amp_return_code.h"
#include "amp_raw_mutex.h"



int amp_raw_mutex_init(amp_mutex_t mutex)
{
    assert(NULL != mutex);
    
    pthread_mutexattr_t mutex_attributes;
    int retval = pthread_mutexattr_init(&mutex_attributes);
    if (0 != retval) {
        if (ENOMEM == retval) {
            return AMP_NOMEM;
        } else {
            return AMP_ERROR;
        }
    }
    
    /* Use an error checking mutex while asserts/debug mode are enabled. */
#if !defined(NDEBUG)
    retval = pthread_mutexattr_settype(&mutex_attributes, 
                                       PTHREAD_MUTEX_ERRORCHECK);
    assert(0 == retval);
#endif
    
    /* Might generate EAGAIN or ENOMEM errors which are handed back to the 
     * caller.
     * EINVAL, EPERM, and EBUSY error codes are implementation problems and are
     * therefore checked while debugging.
     */ 
    retval = pthread_mutex_init(&mutex->mutex, &mutex_attributes);
    switch (retval) {
        case 0:
            /* retval is already equal to AMP_SUCCESS */
            break;
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
    
    /* Get rid of the mutex attribute - it isn't used anymore. */
    int const mattr_destroy_retval = pthread_mutexattr_destroy(&mutex_attributes);
    assert(0 == mattr_destroy_retval);
    (void)mattr_destroy_retval;
    
    return retval;
}



int amp_raw_mutex_finalize(amp_mutex_t mutex)
{
    assert(NULL != mutex);
    
    int retval = pthread_mutex_destroy(&mutex->mutex);
    if (0 != retval) {
        assert(0); /* Programming error */
        retval = AMP_ERROR;
    }
    
    return retval;
}



int amp_mutex_lock(amp_mutex_t mutex)
{
    assert(NULL != mutex);
    
    int retval = pthread_mutex_lock(&mutex->mutex);
    if (0 != retval) {
        assert(0); /* Programming error */
        retval = AMP_ERROR;
    }
    
    return retval;
}



int amp_mutex_trylock(amp_mutex_t mutex)
{
    assert(NULL != mutex);
    
    int retval = pthread_mutex_trylock(&mutex->mutex);
    switch (retval) {
        case 0:
            /* retval is already equal to AMP_SUCCESS */
            break;
        case EBUSY:
            /* retval is already equal to AMP_BUSY */
            break;
        default: /* EINVAL, EDEADLK, EAGAIN - programming error */
            assert(0);
            retval = AMP_ERROR;
    }
    
    return retval;
}



int amp_mutex_unlock(amp_mutex_t mutex)
{
    assert(NULL != mutex);
    
    int retval = pthread_mutex_unlock(&(mutex->mutex));
    if (0 != retval) {
        assert(0); /* Programming error */
        retval = AMP_ERROR;
    }
    
    return retval;
}



/*
 *  amp_raw_mutex_pthreads.c
 *  amp
 *
 *  Created by Björn Knafla on 09.09.09.
 *  Copyright 2009 Bjoern Knafla. All rights reserved.
 *
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
        // assert(ENOMEN != retval && "Insufficient memory for mutex attributes.");
        return retval;
    }
    
    // If supported use an error checking mutex while asserts are enabled.
    //#if defined(_POSIX_THREADS) && !defined(NDEBUG)
    retval = pthread_mutexattr_settype(&mutex_attributes, PTHREAD_MUTEX_ERRORCHECK);
    assert(EINVAL != retval && "Attribute or type invalid.");
    //#endif
    
    retval = pthread_mutex_init(&mutex->mutex, &mutex_attributes);
    assert(EINVAL != retval && "Attribute is invalid.");
        
    // Get rid of the mutex attribute - it isn't used anymore.
    int const mattr_destroy_retval = pthread_mutexattr_destroy(&mutex_attributes);
    assert(EINVAL != mattr_destroy_retval && "Mutex attributes invalid.");

    return retval;
}



int amp_raw_mutex_finalize(amp_raw_mutex_t mutex)
{
    assert(NULL != mutex);
    
    int retval = AMP_SUCCESS;
    retval = pthread_mutex_destroy(&mutex->mutex);
    assert(EINVAL != retval && "Mutex is invalid.");
    assert(EBUSY != retval && "Mutex is in use.");
    
    return retval;
}



int amp_raw_mutex_lock(amp_raw_mutex_t mutex)
{
    assert(NULL != mutex);
    
    int retval = AMP_SUCCESS;
    retval = pthread_mutex_lock(&mutex->mutex);
    assert(EINVAL != retval && "Thread priority exceeds mutex priority ceiling or mutex is invalid.");
    // assert(EDEADLK != retval && "Calling thread already owns mutex.");
    
    return retval;
}



int amp_raw_mutex_trylock(amp_raw_mutex_t mutex)
{
    assert(NULL != mutex);
    
    int retval = AMP_SUCCESS;
    retval = pthread_mutex_trylock(&mutex->mutex);
    assert(EINVAL != retval && "Thread priority exceeds mutex priority ceiling or mutex is invalid.");
    // assert(EBUSY != retval && "Mutex is already locked.");
    // assert(EDEADLK != retval && "Calling thread already owns mutex.");
    
    return retval;
}



int amp_raw_mutex_unlock(amp_raw_mutex_t mutex)
{
    assert(NULL != mutex);
    
    int retval = AMP_SUCCESS;
    retval = pthread_mutex_unlock(&mutex->mutex);
    assert(EINVAL != retval && "Mutex is invalid.");
    // assert(EPERM != retval && "Calling thread does not own mutex.");
    
    return retval;
}





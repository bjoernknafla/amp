/*
 *  amp_raw_semaphore_pthreads.c
 *  amp
 *
 *  Created by Björn Knafla on 03.09.09.
 *  Copyright 2009 Bjoern Knafla. All rights reserved.
 *
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



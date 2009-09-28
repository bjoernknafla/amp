/*
 *  amp_raw_condition_variable.c
 *  amp
 *
 *  Created by Björn Knafla on 28.09.09.
 *  Copyright 2009 Bjoern Knafla Parallelization + AI + Gamedev Consulting. All rights reserved.
 *
 */

#include "amp_raw_condition_variable.h"



#include "amp_raw_mutex.h"



int amp_raw_condition_variable_init(amp_raw_condition_variable_t cond)
{
    assert(NULL != cond);
    
    if (NULL == cond) {
        return EINVAL;
    }
    
    int retval = pthread_cond_init(&cond->cond, NULL);
    assert(EINVAL != retval && "Condition variable attribute is invalid.")
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

    assert(= == retval && "Unexpected error.");
    
    return retval;
}



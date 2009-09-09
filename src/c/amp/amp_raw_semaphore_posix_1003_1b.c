/*
 *  amp_raw_semaphore_posix_1003_1b.c
 *  amp
 *
 *  Created by Björn Knafla on 02.09.09.
 *  Copyright 2009 Bjoern Knafla. All rights reserved.
 *
 */

#include "amp_raw_semaphore.h"

// Include NULL
#include <stddef.h>

// Include assert
#include <assert.h>

// Include errno, EINVAL, ENOSYS, ENOSPC, EPERM, EBUSY, EINTR, EDEADLK
#include <errno.h>


// Include AMP_SUCCESS
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
    /* TODO: @todo only assert or only check for error and return error code? */
    if (!(NULL != sem) ||
        !(0 <= ((unsigned int)init_count)) ||
        !(AMP_RAW_SEMAPHORE_COUNTER_MAX >= init_count)) {
        
        assert(NULL != sem);
        assert(0 <= ((unsigned int)init_count));
        assert(AMP_RAW_SEMAPHORE_COUNT_MAX >= init_count);
        
        return EINVAL;
    }
    
    
    int const retval = sem_init(&sem->semaphore, 
                                0, /* Don't share semaphore between processes */
                                (unsigned int)init_count);
    // assert(0 == retval && "Unknown error code.");
    
    int return_code = AMP_SUCCESS;
    if (0 != retval) {
        assert(EINVAL != errno && "sem->semaphore is not a valid semaphore or init_count exceeds SEM_VALUE_MAX.");
        assert(ENOSPC != errno && "A required resource has been exhausted.");
        assert(ENOSYS != errno && "System does not support semaphores.");
        assert(EPERM != errno && "Process lacks privileges.");
        assert(0 == retval && "Unknown error code.");
        
        return_code = errno;
    }
    
    return return_code;
}



int amp_raw_semaphore_finalize(struct amp_raw_semaphore_s *sem)
{
    /* TODO: @todo only assert or only check for error and return error code? */
    if (!(NULL != sem)) {
        assert(NULL != sem);
        
        return EINVAL;
    }
    
    
    int retval = sem_destroy(&sem->semaphore);

    // assert(0 == retval && "Unknown error code.");
    
    int return_code = AMP_SUCCESS;
    if(0 != retval) {
        assert(ENOSYS != errno && "System does not support semaphores.");
        assert(EBUSY != errno && "Threads are still blocked on the semaphore.");
        assert(0 == retval && "Unknown error code.");
        
        return_code = errno;
    }

    return return_code;
}



int amp_raw_semaphore_wait(struct amp_raw_semaphore_s *sem)
{
    
    /* TODO: @todo only assert or only check for error and return error code? */
    if (!(NULL != sem)) {
        assert(NULL != sem);
        
        return EINVAL;
    }
    
    int retval = sem_wait(&sem->semaphore);
    
    // assert(0 == retval && "Unknown error code.");
    
    int return_code = AMP_SUCCESS;
    if(0 != retval) {
        assert(EINVAL != errno && "sem->semaphore is not a valid semaphore.");
        assert(EINTR != errno && "Waiting was interrupted by a signal.");
        assert(ENOSYS != errno && "System does not support semaphores.");
        assert(EDEADLK != errno && "A deadlock was detected.");
        assert(0 == retval && "Unknown error code.");
        
        return_code = errno;
    }
    
    return return_code;
}


int amp_raw_semaphore_signal(struct amp_raw_semaphore_s *sem)
{
    
    /* TODO: @todo only assert or only check for error and return error code? */
    if (!(NULL != sem)) {
        assert(NULL != sem);
        
        return EINVAL;
    }
    
    int retval = sem_post(&sem->semaphore);
    
    // assert(0 == retval && "Unknown error code.");
    
    int return_code = AMP_SUCCESS;
    if(0 != retval) {
        assert(EINVAL != errno && "sem->semaphore is not a valid semaphore.");
        assert(ENOSYS != errno && "System does not support semaphores.");
        assert(0 == retval && "Unknown error code.");
        
        return_code = errno;
    }
    
    return return_code;    
}




/*
 *  amp_raw_semaphore.c
 *  amp
 *
 *  Created by Björn Knafla on 02.09.09.
 *  Copyright 2009 Bjoern Knafla. All rights reserved.
 *
 */

/**
 * @file
 *
 * TODO: @todo Add a state variable to amp_raw_semaphore_s to help 
 *             detecting use of an uninitialized instance.
 *
 * TODO: @todo Document return codes for all supported platforms.
 */

#ifndef AMP_amp_raw_semaphore_H
#define AMP_amp_raw_semaphore_H



#if defined(AMP_USE_POSIX_1003_1B_SEMAPHORES)
#   include <semaphore.h>
#elif defined(AMP_USE_PTHREADS)
#   include <pthread.h>
#   include <limits.h>
#elif defined(AMP_USE_WINTHREADS)
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#else
#   error Unsupported platform.
#endif




#if defined(__cplusplus)
extern "C" {
#endif
    
    
    /**
     * Type used to specify the semaphore start count when initialized.
     * 
     * When changing the type also adapt AMP_RAW_SEMAPHORE_COUNT_MAX
     * and check if limits.h must be included or not.
     */
    typedef unsigned int amp_raw_semaphore_count_t;
    
    /**
     * @def  AMP_RAW_SEMAPHORE_COUNT_MAX
     *
     * Maximal value of the internal semaphore counter to not-block threads
     * when waiting on the semaphore.
     */
#if defined(AMP_USE_POSIX_1003_1B_SEMAPHORES)
#   define AMP_RAW_SEMAPHORE_COUNT_MAX (amp_raw_semaphore_count_t)(SEM_VALUE_MAX)
#elif defined(AMP_USE_PTHREADS)
#   define AMP_RAW_SEMAPHORE_COUNT_MAX (amp_raw_semaphore_count_t)(UINT_MAX)
#elif defined(AMP_USE_WINTHREADS)
#   define AMP_RAW_SEMAPHORE_COUNT_MAX (amp_raw_semaphore_count_t)(MAX_SEM_COUNT)
#else
#   error Unsupported platform.
#endif
    
    
    /**
     * Must be initialized before usage and finalized to free reserved 
     * resources.
     */
    struct amp_raw_semaphore_s {
#if defined(AMP_USE_POSIX_1003_1B_SEMAPHORES)
        sem_t semaphore;
#elif defined(AMP_USE_PTHREADS)
        pthread_mutex_t mutex;
        pthread_cond_t a_thread_can_pass;
        amp_raw_semaphore_count_t count;
#elif defined(AMP_USE_WINTHREADS)
        HANDLE semaphore_handle;
#else
#   error Unsupported platform.
#endif
    };
    typedef struct amp_raw_semaphore_s *amp_raw_semaphore_t;
    
    
    /**
     * sem mustn't be NULL.
     * init_count mustn't be negative and mustn't be greater than
     * AMP_RAW_SEMAPHORE_COUNT_MAX.
     */
    int amp_raw_semaphore_init(struct amp_raw_semaphore_s *sem,
                                         amp_raw_semaphore_count_t init_count);
    
    /**
     *
     * sem mustn't be NULL.
     */
    int amp_raw_semaphore_finalize(struct amp_raw_semaphore_s *sem);
    
    /**
     * If the semaphore counter is not zero decrements the counter and pass the
     * semaphore. If the counter is zero the thread blocks until the semaphore
     * counter becomes greater than zero again and its the threads turn to 
     * decrease and pass it.
     *
     * sem mustn't be NULL.
     *
     * TODO: @todo Decide if os signals should be able to interrupt the waiting.
     */
    int amp_raw_semaphore_wait(struct amp_raw_semaphore_s *sem);
    
    /**
     * Increments the semaphore counter by one and if threads are blocked on the
     * semaphore one of them is woken up and gets the chance to decrease the 
     * counter and pass the semaphore.
     *
     * sem mustn't be NULL.
     */
    int amp_raw_semaphore_signal(struct amp_raw_semaphore_s *sem);
    
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif
    

#endif /* AMP_amp_raw_semaphore_H */

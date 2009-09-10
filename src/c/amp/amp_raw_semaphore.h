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

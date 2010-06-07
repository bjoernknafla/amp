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
 * Definition of amp_raw_semaphore_s containing backend dependencies.
 *
 * @attention Don't copy or move raw semaphores - copying and moving pointers to
 *            them is ok though you need to take care about ownership 
 *            management.
 *
 * @attention ENOSYS might be returned by the functions if the POSIX 1003 1b 
 *            backend is used and the system doesn't support semaphores. Use 
 *            another backend while compiling, for example AMP_USE_WINTHREADS,
 *            or AMP_USE_PTHREADS and don't define
 *            AMP_USE_POSIX_1003_1b_SEMAPHORES.
 *
 * TODO: @todo Add a state variable to amp_raw_semaphore_s to help 
 *             detecting use of an uninitialized instance.
 *
 * TODO: @todo Document return codes for all supported platforms.
 *
 * TODO: @todo When adding a trywait function look if POSIX specifies EBUSY
 *             or EAGAIN as a return value to indicate that the thread would
 *             block.
 *
 * TODO: @todo Add a way to ensure that the calls to signal and wait are 
 *             balanced before a semaphore is finalized or detroyed.
 */

#ifndef AMP_amp_raw_semaphore_H
#define AMP_amp_raw_semaphore_H

#include <amp/amp_semaphore.h>

#if defined(AMP_USE_POSIX_1003_1B_SEMAPHORES)
#   include <semaphore.h>
#elif defined(AMP_USE_LIBDISPATCH_SEMAPHORES)
#   include <dispatch/dispatch.h>
#   include <limits.h>
#elif defined(AMP_USE_PTHREADS)
#   include <pthread.h>
#   include <limits.h>
#elif defined(AMP_USE_WINTHREADS)
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include <limits.h>
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
#if defined(AMP_USE_POSIX_1003_1B_SEMAPHORES)
    typedef unsigned int amp_raw_semaphore_counter_t;
#elif defined(AMP_USE_LIBDISPATCH_SEMAPHORES)
    typedef long amp_raw_semaphore_counter_t;
#elif defined(AMP_USE_PTHREADS)
    typedef unsigned int amp_raw_semaphore_counter_t;
#elif defined(AMP_USE_WINTHREADS)
    typedef long amp_raw_semaphore_counter_t;
#else
#   error Unsupported platform.
#endif

    /**
     * @def  AMP_RAW_SEMAPHORE_COUNT_MAX
     *
     * Maximal value of the internal semaphore counter to not-block threads
     * when waiting on the semaphore.
     *
     * TODO: @todo Try to find a Windows platform constant for the max number
     *             of semaphores allowed (not found yet).
     */
#if defined(AMP_USE_POSIX_1003_1B_SEMAPHORES)
#   define AMP_RAW_SEMAPHORE_COUNT_MAX ((amp_raw_semaphore_counter_t)(SEM_VALUE_MAX))
#elif defined(AMP_USE_LIBDISPATCH_SEMAPHORES)
#   define AMP_RAW_SEMAPHORE_COUNT_MAX ((amp_raw_semaphore_counter_t)(LONG_MAX))
#elif defined(AMP_USE_PTHREADS)
#   define AMP_RAW_SEMAPHORE_COUNT_MAX ((amp_raw_semaphore_counter_t)(LONG_MAX))
#elif defined(AMP_USE_WINTHREADS)
#   define AMP_RAW_SEMAPHORE_COUNT_MAX ((amp_raw_semaphore_counter_t)(LONG_MAX))
#else
#   error Unsupported platform.
#endif
    
    
    /**
     * Must be initialized before usage and finalized to free reserved 
     * resources.
     *
     * @attention Don't copy or move, otherwise behavior is undefined.
     */
    struct amp_raw_semaphore_s {
#if defined(AMP_USE_POSIX_1003_1B_SEMAPHORES)
        sem_t semaphore;
#elif defined(AMP_USE_LIBDISPATCH_SEMAPHORES)
        dispatch_semaphore_t semaphore;
#elif defined(AMP_USE_PTHREADS)
        pthread_mutex_t mutex;
        pthread_cond_t a_thread_can_pass;
        amp_semaphore_counter_t count;
#elif defined(AMP_USE_WINTHREADS)
        HANDLE semaphore_handle;
#else
#   error Unsupported platform.
#endif
    };
    
    
    /**
     * Like amp_semaphore_create but does not allocate memory for the semaphore
     * other than indirectly via the platform API to create one.
     */
    int amp_raw_semaphore_init(struct amp_raw_semaphore_s *sem,
                               amp_semaphore_counter_t init_count);
    
    
    /**
     * Like amp_semaphore_destroy but does not free memory for the amp semaphore
     * other than indirectly via the platform API to destroy one.
     */
    int amp_raw_semaphore_finalize(struct amp_raw_semaphore_s *sem);
    
    
    
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif
    

#endif /* AMP_amp_raw_semaphore_H */

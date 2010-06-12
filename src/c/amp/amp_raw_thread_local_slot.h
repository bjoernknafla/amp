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
 * Definition of amp_raw_thread_local_slot_key containing backend 
 * dependencies.
 */

#ifndef AMP_amp_raw_thread_local_slot_H
#define AMP_amp_raw_thread_local_slot_H

#include <amp/amp_thread_local_slot.h>



#if defined(AMP_USE_PTHREADS)
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


    /* Minimal available slots that are for sure usable on the platform. */
#if defined(AMP_USE_PTHREADS)
#   define AMP_RAW_THREAD_LOCAL_SLOT_COUNT_AVAILABLE_FOR_SURE PTHREAD_KEYS_MAX
/* #   define AMP_RAW_THREAD_LOCAL_SLOT_COUNT_POSSIBLY_AVAILABLE_MAX PTHREADS_KEY_MAX */
#elif defined(AMP_USE_WINTHREADS)
#   define AMP_RAW_THREAD_LOCAL_SLOT_COUNT_AVAILABLE_FOR_SURE TLS_MINIMUM_AVAILABLE
/* #   define AMP_RAW_THREAD_LOCAL_SLOT_COUNT_POSSIBLY_AVAILABLE_MAX 1088 */
#else
#   error Unsupported platform.
#endif    
    
    struct amp_raw_thread_local_slot_key_s {
#if defined(AMP_USE_PTHREADS)
        pthread_key_t key;
#elif defined(AMP_USE_WINTHREADS)
        DWORD tls_index;
#else
#   error Unsupported platform.
#endif
    };
    
        
    
    /**
     * Like amp_thread_local_slot_create but does not allocate memory for 
     * the thread-specific storage other than indirectly via the platform API to 
     * create thread-specific storage.
     */
    int amp_raw_thread_local_slot_init(amp_thread_local_slot_key_t key);
    
    
    /**
     * Like amp_thread_local_slot_destroy but does not free memory other 
     * than indirectly via the platform API to destroy the thread-specific 
     * storage.
     */
    int amp_raw_thread_local_slot_finalize(amp_thread_local_slot_key_t key);

    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif


#endif /* AMP_amp_raw_thread_local_slot_H */

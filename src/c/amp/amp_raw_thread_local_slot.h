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
 *
 */


#ifndef AMP_amp_raw_thread_local_slot_H
#define AMP_amp_raw_thread_local_slot_H

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


    
#if defined(AMP_USE_PTHREADS)
#   define AMP_RAW_THREAD_LOCAL_SLOT_COUNT_MIN PTHREADS_KEY_MAX
#   define AMP_RAW_THREAD_LOCAL_SLOT_COUNT_MAX PTHREADS_KEY_MAX
#elif defined(AMP_USE_WINTHREADS)
#   error Not implemented yet.
#else
#   error Unsupported platform.
#endif    
    
    struct amp_raw_thread_local_slot_key_s {
#if defined(AMP_USE_PTHREADS)
        pthread_key_t key;
#elif defined(AMP_USE_WINTHREADS)
#   error Not implemented yet.
#else
#   error Unsupported platform.
#endif
    };
    
    typedef struct amp_raw_thread_local_slot_key_s amp_raw_thread_local_slot_key_t;
    
    
    int amp_raw_thread_local_slot_init(amp_raw_thread_local_slot_key_t *key);
    
    /**
     * Finalizing a slot doesn't deallocate the memory that might be
     * associated with the data stored in the different thread local slots,
     * therefore remvoe the data on all threads first if necessary.
     */
    int amp_raw_thread_local_slot_finalize(amp_raw_thread_local_slot_key_t key);
    
    int amp_raw_thread_local_slot_set_value(amp_raw_thread_local_slot_key_t key,
                                                      void *value);
    
    void* amp_raw_thread_local_slot_get_value(amp_raw_thread_local_slot_key_t key);
    
    
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif


#endif /* AMP_amp_raw_thread_local_slot_H */

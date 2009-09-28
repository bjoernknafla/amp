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
 * Shallow wrapper around Pthreads thread-specific data or Windows thread-local
 * storage.
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
    
    typedef struct amp_raw_thread_local_slot_key_s amp_raw_thread_local_slot_key_t;
    
    /**
     * Creates a thread-local slot for all threads (including the main thread)
     * whose value can be accessed via the identifier stored in @a key. The 
     * initial value in the slot belonging to key is NULL.
     *
     * Each thread has to set the value of its own slot.
     *
     * @return AMP_SUCCESS on successful slot creation.
     *         EAGAIN if system resource are insufficient or the max slot count
     *         has been exceeded.
     *         ENOMEM if the system has insufficient memory to create the key.
     *         Error codes might be returned to signal errors while
     *         finalization, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         EINVAL if the key can't be stored in the argument.
     *
     * @attention key mustn't be NULL.
     */
    int amp_raw_thread_local_slot_init(amp_raw_thread_local_slot_key_t *key);
    
    /**
     * Invalidates @a key.
     *
     * Finalizing a slot doesn't deallocate the memory that might be
     * associated with the data stored in the different thread local slots,
     * therefore remove the data on all threads first if necessary.
     *
     * @return AMP_SUCCESS on successful finalization.
     *         Error codes might be returned to signal errors while
     *         finalization, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         EINVAL if the key is invalid.
     *
     * @attention Trying to access a slot via an invalid, e.g. finalized key,
     *            results in undefined behavior.
     */
    int amp_raw_thread_local_slot_finalize(amp_raw_thread_local_slot_key_t key);
    
    /**
     * Store a value in the slot identified by key.
     *
     * @return AMP_SUCCESS on success.
     *         ENOMEM if the system has insufficient memory to associate the
     *         value with the key.
     *         Error codes might be returned to signal errors while
     *         setting the value, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         EINVAL if the key is invalid.
     *
     * @attention NULL is a special value - for @em amp it means that the slot
     *            is empty.
     *
     * @attention Trying to access a slot via an invalid, e.g. finalized key,
     *            results in undefined behavior.
     */
    int amp_raw_thread_local_slot_set_value(amp_raw_thread_local_slot_key_t key,
                                                      void *value);

    /**
     * Query the slot via its key for its value.
     *
     * @return Current value stored in the slot associated with the key for the
     *         thread calling the function or NULL if the slot is empty.
     *
     * @attention Trying to access a slot via an invalid, e.g. finalized key,
     *            results in undefined behavior.
     */
    void* amp_raw_thread_local_slot_get_value(amp_raw_thread_local_slot_key_t key);
    
    
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif


#endif /* AMP_amp_raw_thread_local_slot_H */

/*
 *  amp_raw_thread_local_slot.h
 *  amp
 *
 *  Created by Björn Knafla on 07.09.09.
 *  Copyright 2009 Bjoern Knafla. All rights reserved.
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

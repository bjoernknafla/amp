/*
 *  amp_raw_mutex.h
 *  amp
 *
 *  Created by Björn Knafla on 08.09.09.
 *  Copyright 2009 Bjoern Knafla. All rights reserved.
 *
 */

#ifndef AMP_amp_raw_mutex_H
#define AMP_amp_raw_mutex_H



#if defined(AMP_USE_PTHREADS)
#   include <pthread.h>
#elif defined(AMP_USE_WINTHREADS)
#   define WIN32_LEAN_AND_MEAN
#   error Not implemented yet.
#else
#   error Unsupported platform.
#endif




#if defined(__cplusplus)
extern "C" {
#endif


    struct amp_raw_mutex_s {
#if defined(AMP_USE_PTHREADS)
        pthread_mutex_t mutex;
#   if defined _POSIX_THREADS
        // pthread_t locking_thread_debug_helper;
#   endif /* _POSTIX_THREADS */
        
        
#elif defined(AMP_USE_WINTHREADS)
#   error Not implemented yet.
#else
#   error Unsupported platform.
#endif
    };
    
    // Simple mutex - doesn't allow recursive locking.
    typedef struct amp_raw_mutex_s *amp_raw_mutex_t;
    
    int amp_raw_mutex_init(amp_raw_mutex_t mutex);
    int amp_raw_mutex_finalize(amp_raw_mutex_t mutex);
    
    int amp_raw_mutex_lock(amp_raw_mutex_t mutex);
    int amp_raw_mutex_trylock(amp_raw_mutex_t mutex);
    int amp_raw_mutex_unlock(amp_raw_mutex_t mutex);
    

#if defined(__cplusplus)
} /* extern "C" */
#endif

        
#endif /* AMP_amp_raw_mutex_H */

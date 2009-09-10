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
 * Simple non-recursive mutex and a shallow wrapper around the platforms mutex 
 * or critical section primitive.
 *
 * amp_raw_mutex guarantees that only one thread can lock it at a time.
 * amp_raw_mutex offers a way to synchronize access to a so called critical
 * section wrapped in a mutex lock and unlock operation. As only one thread
 * can be inside the critical section at a time, all shared resources
 * accessed are protected from race conditions that could happen if another
 * thread would run the instructions of the critical section concurrently.
 *
 * The thread locking the mutex attains its ownership and needs to unlock
 * it, too. As long as a thread holds the mutex lock no other thread can
 * lock it and will block and wait on the mutex to be unlocked. The moment 
 * the mutex is unlocked from its previous holder thread one thread that is 
 * blocked on the mutex will be woken up and it - or anothread that slips 
 * in calling lock - gets the lock. If the just woken up thread doesn't get 
 * the lock it will be blocked and waits on the lock again.
 * Which thread actually attain the lock is non-deterministic and platform
 * dependent. Don't rely on any order.
 * 
 * Before using a mutex it must be initialized via amp_raw_mutex_init once.
 * Don't forget to finalize the mutex and free its memory and OS internal
 * resources by calling amp_raw_mutex_finalize at the end of the mutex
 * lifetime.
 *
 * All functions working on mutexes return error codes. Check these return
 * codes and don't enter a critical section or use a mutex if the return
 * value isn't AMP_SUCCESS.
 *
 * @attention If the thread already holding the mutex lock calls
 *            amp_raw_mutex_lock recursively the behavior is undefined
 *            and might lead to a deadlock or possibly a value of
 *            EDEADLK or EBUSY is returned. Don't lock recursively.
 *            If the sources are compiled without defining NDEBUG an 
 *            internal assertion will trigger.
 *
 *
 * TODO: @todo Rework the documentation to be more concise. 
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

    /**
     * Simple non-recursive mutex.
     */
    struct amp_raw_mutex_s {
#if defined(AMP_USE_PTHREADS)
        pthread_mutex_t mutex;
#   if defined _POSIX_THREADS
        /* pthread_t locking_thread_debug_helper; */
#   endif /* _POSTIX_THREADS */
#elif defined(AMP_USE_WINTHREADS)
#   error Not implemented yet.
#else
#   error Unsupported platform.
#endif
    };
    
    /**
     * Simple non-recursive mutex.
     * See amp_raw_mutex_s.
     */
    typedef struct amp_raw_mutex_s *amp_raw_mutex_t;
    
    /**
     * Initialize amp_raw_mutex_t before using it.
     *
     * Calling amp_raw_mutex_init on an already initialized and non-finalized
     * mutex results in undefined behavior.
     *
     * @return AMP_SUCCESS is returned on successful initialization of mutex.
     *         EAGAIN is returned if the system temporarily has insufficent 
     *                resources.
     *         ENOMEM is returned if memory is insufficient.
     *         Other error codes might be returned to signal errors while
     *         initializing, too.
     */
    int amp_raw_mutex_init(amp_raw_mutex_t mutex);
    int amp_raw_mutex_finalize(amp_raw_mutex_t mutex);
    
    int amp_raw_mutex_lock(amp_raw_mutex_t mutex);
    int amp_raw_mutex_trylock(amp_raw_mutex_t mutex);
    int amp_raw_mutex_unlock(amp_raw_mutex_t mutex);
    

#if defined(__cplusplus)
} /* extern "C" */
#endif

        
#endif /* AMP_amp_raw_mutex_H */

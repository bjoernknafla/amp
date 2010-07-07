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
 * Simple non-recursive mutex and a shallow wrapper around the platforms mutex 
 * or critical section primitive.
 *
 * amp mutex guarantees that only one thread can lock it at a time.
 * amp_mutex_t offers a way to synchronize access to a so called critical
 * section wrapped in a mutex lock and unlock operation. As only one thread
 * can be inside the critical section at a time, all shared resources accessed 
 * inside the critical section are protected from race conditions that could 
 * happen if another thread would execute the critical section concurrently.
 *
 * @attention If someone accesses resources without using the single shared
 *            mutex associated with it to protect it, then race conditions can
 *            and will happen.
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
 * Before using a mutex it must be created via amp_mutex_create once.
 * Don't forget to destroy the mutex to free its memory and OS internal
 * resources by calling amp_mutex_destroy at the end of the mutex
 * lifetime.
 *
 * All functions working on mutexes return return codes. Check these return
 * codes and don't enter a critical section or use a mutex if the return
 * value isn't AMP_SUCCESS.
 *
 * @attention If the thread already holding the mutex lock calls
 *            amp_mutex_lock recursively the behavior is undefined
 *            and might lead to a deadlock.
 *
 * @attention If a thread that isn't holding a mutex lock tries to unlock the
 *            mutex behavior is undefined.
 *
 * @attention If a invalid mutex is passed to any function behavior is 
 *            undefined. More specifically, never pass an uninitialized (or 
 *            after initialization finalized) mutex to any function other than
 *            amp_mutex_create. Never pass an initialized mutex to 
 *            amp_mutex_create.
 *
 *
 * TODO: @todo Rework the documentation to be more concise. 
 *
 * TODO: @todo At least on Windows store the locking threads id to detect
 *             if wrong thread tries to unlock in debug mode.
 */

#ifndef AMP_amp_mutex_H
#define AMP_amp_mutex_H

#include <stddef.h>

#include <amp/amp_memory.h>



#if defined(__cplusplus)
extern "C" {
#endif

    
#define AMP_MUTEX_UNINITIALIZED NULL
    
    
    /**
     * Simple non-recursive mutex.
     * See amp_raw_mutex_s.
     * Can be moved or copied - but every copy identifies the same mutex - 
     * manage ownership and reference counts yourself.
     */
    typedef struct amp_raw_mutex_s *amp_mutex_t;
    
    
    
    /**
     * Allocates and initialize an amp_mutex_t before using it.
     *
     * @attention The platform-specific init function called might use malloc
     *            or other resource management functions internally.
     *
     * @attention Don't pass an already created mutex to amp_mutex_create
     *            as it could result in undefined behavior and resource leaks.
     *
     * If the initialization fails the allocator is called to free the
     * already allocated memory which must not result in an error or otherwise
     * behavior is undefined.
     *
     * @return AMP_SUCCESS is returned on successful initialization of mutex.
     *         AMP_ERROR is returned if the system temporarily has insufficent 
     *         resources.
     *         AMP_NOMEM is returned if memory is insufficient.
     *         Other error codes might be returned to signal errors while
     *         initializing, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR might be returned when trying to create a mutex
     *         while the argument mutex points to an already exiting one.
     */
    int amp_mutex_create(amp_mutex_t* mutex,
                         amp_allocator_t allocator);
    
    /**
     * Finalizes the mutex and frees its memory and platform resources.
     *
     * @return AMP_SUCCESS on successful destruction of the mutex.
     *         Error codes might be returned to signal errors while
     *         finalizing, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR if the mutex attribute isn't valid, e.g. not 
     *         initialized.
     *         AMP_BUSY if the mutex is locked by a thread.
     *
     * allocator must be capable of freeing the memory
     * allocated via the create function otherwise behavior is undefined and
     * resources might be leaked.
     *
     * @attention Only call for successfully initialized mutexes, otherwise
     *            behavior is undefined.
     *
     * @attention Only call for mutexes which aren't locked by any thread and 
     *            for which no threads are blocked waiting on the lock,
     *            otherwise behavior is undefined.
     *
     * @attention The platform-specific destruction function called might use 
     *            free or other resource management functions internally.
     */
    int amp_mutex_destroy(amp_mutex_t* mutex,
                          amp_allocator_t allocator);
    
    /**
     * Locks the mutex or, if another thread holds the lock, blocks waiting 
     * until it gathers the mutex lock.
     *
     * @return AMP_SUCCESS is returned if locking is successful.
     *         Error codes might be returned to signal errors while
     *         locking, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR if the thread already holding the lock attempts to lock
     *         again (recursively) or if the mutex is invalid, e.g. not 
     *         initialized.
     *
     * @attention Trying to recursively lock a mutex from the same thread
     *            results in undefined behavior. Never lock recursively.
     */
    int amp_mutex_lock(amp_mutex_t mutex);
    
    /**
     * Locks the mutex or, if the mutex is already locked by another thread,
     * returns with an error code.
     *
     * @return AMP_SUCCESS if the lock has been taken.
     *         AMP_BUSY if lock hasn't been taken because it is locked by  
     *         another thread.
     *         Error codes might be returned to signal errors while
     *         trying to lock, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR if the mutex isn't valid, e.g. not initialized or if 
     *         trying to lock recursively.
     *
     * @attention Don't enter the critical section if an error code is returned
     *            because the lock hasn't been taken.
     *
     * @attention Trying to recursively lock a mutex from the same thread
     *            results in undefined behavior. Never lock recursively.
     */
    int amp_mutex_trylock(amp_mutex_t mutex);
    
    /**
     * Unlocks the mutex, other threads trying to lock it or which are blocked
     * waiting on the lock will get it in non-deterministic order one after the
     * other afterwards.
     *
     * @return AMP_SUCCESS after successful unlocking.
     *         Error codes might be returned to signal errors while
     *         unlocking, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR if the mutex isn't valid, e.g. not initialized or if a 
     *         thread not holding the lock tries to unlock the mutex.
     *
     * @attention Only the thread holding the lock is allowed to unlock it, 
     *            otherwise behavior is undefined.
     *
     * @attention Trying to unlock an already unlocked mutex results in 
     *            undefined behavior.
     */
    int amp_mutex_unlock(amp_mutex_t mutex);
    
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif
    

#endif /* AMP_amp_mutex_H */

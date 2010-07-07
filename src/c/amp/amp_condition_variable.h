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
 * Simple implementation that behaves alike POSIX threads condition variables.
 *
 * Never pass an invalid or NULL argument to the functions.
 * Never pass a non-created condition variable to the functions other than
 * the create function. Never pass a created function to the create function
 * if it hasn't been destroyed before.
 * Don't destroy a condition variable that is still in use by threads, e.g.
 * while threads are waiting on it or might signal it later on.
 * Creating a condition variable must only be done by a single thread and 
 * destroying a condition variable must only be done by a single thread, it 
 * isn't thread-safe.
 */

#ifndef AMP_amp_condition_variable_H
#define AMP_amp_condition_variable_H

#include <stddef.h>

#include <amp/amp_mutex.h>
#include <amp/amp_memory.h>


#if defined(__cplusplus)
extern "C" {
#endif

#define AMP_CONDITION_VARIABLE_UNINITIALIZED NULL
    
    typedef struct amp_raw_condition_variable_s* amp_condition_variable_t;
    
    
    /**
     * Allocates and initialized a condition variable.
     *
     * Only call from one thread.
     *
     * Behavior is undefined if calling it on an already created and not 
     * destroyed condition variable - resources will be leaked or worser.
     *
     * If the initialization fails the allocator is called to free the
     * already allocated memory which must not result in an error or otherwise
     * behavior is undefined.
     *
     * @return AMP_SUCCESS if initialized successful, otherwise error codes are
     *         returned.
     *         AMP_ERROR if a system resource other than memory wasn't 
     *         available.
     *         AMP_NOMEM if not enough memory is available.
     *         Error codes might be returned to signal errors while
     *         initializing, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_BUSY if the condition variable is already initialized.
     *         AMP_ERROR if the condition variable is invalid or already 
     *         initialized.
     */
    int amp_condition_variable_create(amp_condition_variable_t* cond,
                                      amp_allocator_t allocator);
    
    /**
     * Finalizes and frees the memory of a condition variable.
     *
     * Only call from one thread when the condition variable isn't in use
     * anymore.
     *
     * allocator must be capable of freeing the memory allocated via the create 
     * function otherwise behavior is undefined and resources might be leaked.
     *
     * @return AMP_SUCCESS if the condition variable was successfully destroyed.
     *         Error codes might be returned to signal errors while
     *         finalizing, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR the condition variable is invalid or allocator could
     *         not deallocate the condition variable.
     *         AMP_BUSY the condition variable is in use.
     */
    int amp_condition_variable_destroy(amp_condition_variable_t* cond,
                                       amp_allocator_t allocator);
    
    /**
     * Wakes up all threads waiting on the condition variable. Only one thread
     * will be able to get the lock of the associated mutex (see wait function
     * below), all other will block while waiting on the lock. The "winner"
     * thread will leave its call to amp_condition_variable_wait with the
     * associated mutex locked by it.
     * The thread calling broadcast can but needn't own the lock on the 
     * associated mutex.
     *
     * @return AMP_SUCCESS on successful broadcasting.
     *         Error codes might be returned to signal errors while
     *         broadcasting, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR if the condition variable isn't valid.
     */
    int amp_condition_variable_broadcast(amp_condition_variable_t cond);
    
    /**
     * Wakes up an unspecified single waiting thread of the threads waiting on
     * the condition variable. The woken up thread will leave the 
     * call to amp_condition_variable_wait with the associated mutex locked.
     * The thread calling signal can but needn't own the lock on the associated
     * mutex.
     *
     * @return AMP_SUCCESS on successful signaling.
     *         Error codes might be returned to signal errors while
     *         broadcasting, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR if the condition variable isn't valid.
     */
    int amp_condition_variable_signal(amp_condition_variable_t cond);
    
    /**
     * The thread calling amp_condition_variable_wait waits on the 
     * conditiona variable until awaken by a signal or broadcast. While waiting
     * the mutex is unlocked. When awaking
     * the associated mutex is re-locked. The thread should then re-check 
     * that the predicate or state condition it waited on via the condition
     * variable is true or should wait on the condition variable again 
     * otherwise.
     * The thread calling must own the lock on the mutex assocaited with the
     * condition variable.
     *
     * @attention Only call if the mutex is locked by the calling thread,
     *            otherwise behavior is undefined.
     *
     * @attention A signal or broadcast can be missed when
     *            signal or broadcast are called without owning the mutex
     *            when the signal or broadcast is issued before the wait
     *            registered itself to wait on the condition variable.
     *            This doesn't hapen if signal or broadcast are called while
     *            owning the mutex.
     *
     * @return AMP_SUCCESS after the calling thread has been awoken by a signal 
     *         or broadcast and has already locked the associated mutex.
     *         Error codes might be returned to signal errors while
     *         waiting, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR is the condition variable or the mutex are invalid, or
     *         if different mutexes are used for concurrent waits on the same
     *         condition variable, or if the mutex isn't owned by the calling
     *         thread.
     */
    int amp_condition_variable_wait(amp_condition_variable_t cond,
                                    amp_mutex_t mutex);
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif


#endif /* AMP_amp_condition_variable_H */

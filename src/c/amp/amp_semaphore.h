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
 * Shallow wrapper around a semaphore type of the platform.
 * An amp_semaphore can not be shared between two processes, each process only
 * has access to its own semaphores.
 *
 * @attention Don't pass pointers to an invalid semaphore to any
 *            of the functions. Don't pass non-initialized (or after 
 *            initialization destroyed) semaphores to any function other than
 *            to amp_raw_semaphore_init. Don't pass initialized semaphores
 *            to amp_raw_semaphore_init.
 * 
 * TODO: @todo Add amp_semaphore_trywait.
 * TODO: @todo Decide if to add amp_semaphore_max_count.
 */

#ifndef AMP_amp_semaphore_H
#define AMP_amp_semaphore_H

#include <stddef.h>

#include <amp/amp_stdint.h>
#include <amp/amp_memory.h>



#if defined(__cplusplus)
extern "C" {
#endif


#define AMP_SEMAPHORE_UNINITIALIZED NULL
    
    /**
     * Abstract semaphore type.
     */
    typedef struct amp_raw_semaphore_s *amp_semaphore_t;
    
    /**
     * Seamphore counter type.
     */
    typedef intptr_t amp_semaphore_counter_t;

    
    /**
     * Creates a semaphore by allocating memory and initializing it with
     * a counter value of init_count and assigns the result to where semaphore
     * points.
     *
     * Internal calls to the platform semaphore API can result in memory 
     * allocations that bypass the argument supplied allocator.
     *
     * Platforms limit the number of semaphores that can be created and
     * active concurrently - creating too many semaphore won't succeed and an
     * error code will be returned.
     *
     * Platforms and amp limit the maximum value a semaphore count can carry.
     * If an overflow (or underflow) would happen the function called won't
     * succeed and an error code is returned.
     *
     * If the initialization fails the allocator is called to free the
     * already allocated memory which must not result in an error or otherwise
     * behavior is undefined.
     *
     * @return AMP_SUCCESS on successful initialization, otherwise:
     *         AMP_NOMEM if memory is insufficient.
     *         AMP_ERROR if other system resources are insufficient, e.g. too
     *         many semaphores have been created.
     *         AMP_UNSUPPORTED if the backend in use doesn't support semaphores. 
     *         Use another backend while building the library.
     *         Other error codes might be returned to signal errors while
     *         initializing, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR if the semaphore is invalid, the init_count is negative 
     *         or greater than AMP_RAW_SEMAPHORE_COUNT_MAX.
     *         AMP_ERROR if the process lacks privileges to initialize the 
     *         semaphore or if the semaphore is in use.
     *
     * @attention semaphore mustn't be NULL.
     *
     * @attention Don't pass an initialized (and not finalized after 
     *            initialization) semaphore to amp_semaphore_create.
     *
     * @attention init_count mustn't be negative and mustn't be greater than
     *            AMP_RAW_SEMAPHORE_COUNT_MAX.
     */
    int amp_semaphore_create(amp_semaphore_t* semaphore,
                             amp_allocator_t allocator,
                             amp_semaphore_counter_t init_count);
    
    
    /**
     * Finalizes and destroys semaphore and frees the resources it used.
     *
     * The platform semaphore API's finalization routines might bypass the
     * user provided allocator.
     *
     * On destruction the semaphore counter should be equal to the init count
     * set on creation, therefore balance signaling and waiting.
     *
     * allocator_context and dealloc_func must be capable of freeing the memory
     * allocated via the create function otherwise behavior is undefined and
     * resources might be leaked.
     *
     * @return AMP_SUCCESS on successful finalization and destruction.
     *         AMP_UNSUPPORTED if the backend doesn't support semaphores.
     *         Error codes might be returned to signal errors while
     *         finalization, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_BUSY if threads block on the semaphore.
     *         AMP_ERROR if the semaphore isn't valid, e.g. not initialized.
     *
     * @attention semaphore mustn't be NULL.
     *
     * @attention Don't pass an uninitialized semaphore into 
     *            amp_semaphore_destroy.
     *
     * @attention Don't call on a blocked semaphore, otherwise behavior is 
     *            undefined.
     */
    int amp_semaphore_destroy(amp_semaphore_t* semaphore,
                              amp_allocator_t allocator);
    
    
    
    /**
     * If the semaphore counter is not zero decrements the counter and returns.
     * If the counter is zero the thread blocks until the semaphore
     * counter becomes greater than zero again and its the threads turn to 
     * decrease and it and return to work on.
     *
     * @return AMP_SUCCESS after waited successful on the semaphore.
     *         AMP_ERROR if the semaphore was interrupted by a signal when using
     *         a backend that supports signal interruption.
     *         AMP_UNSUPPORTED if the backend doesn't support semaphores.
     *         Error codes might be returned to signal errors while
     *         waiting, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR if a deadlock condition was detected, or if the 
     *         semaphore isn't valid, e.g. not initialized, or if the process 
     *         lacks privileges to wait on the semaphore.
     *
     * @attention sem mustn't be NULL.
     *
     * @attention Based on the backend amp_raw_semaphores might or might not 
     *            react to / are or are not usable with signals. Set the threads
     *            signal mask to not let any signals through.
     *
     * TODO: @todo Decide if OS signals should be able to interrupt the waiting.
     */
    int amp_semaphore_wait(amp_semaphore_t semaphore);
    
    
    /**
     * Increments the semaphore counter by one and if threads are blocked on the
     * semaphore one of them is woken up and gets the chance to decrease the 
     * counter and pass the semaphore to execute on.
     *
     * @return AMP_SUCCESS after succesful signaling the semaphore.
     *         AMP_UNSUPPORTED if the backend doesn't support semaphores.
     *         AMP_ERROR if the semaphore counter value exceeds 
     *           AMP_RAW_SEMAPHORE_COUNT_MAX .
     *         Error codes might be returned to signal errors while
     *         signaling, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR if the semaphore isn't valid, e.g. not initialized, or
     *         if a deadlock condition is detected, or if the process lacks 
     *         privileges to signal the semaphore.
     *
     * @attention semaphore mustn't be NULL.
     */
    int amp_semaphore_signal(amp_semaphore_t semaphore);
    
    
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif


#endif /* AMP_amp_semaphore_H */

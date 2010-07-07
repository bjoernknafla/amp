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
 * Simple thread barrier which is created with a specified numbers of threads
 * which need to wait on the barrier concurrently to be able to pass it all
 * together. Every thread waiting on the barrier decreases an internal counter.
 * When that counter reaches zero the barrier is fulfilled and all threads are
 * unblocked to proceed. One and only one thread will get a return value of
 * AMP_BARRIER_SERIAL_THREAD from the waiting function. This can be used for
 * example in the following case where all other threads enter the next barrier
 * immediately after passing a preceeding one while the thread receiving the 
 * special return value from wait prepares some data by its own and then
 * enters the next barrier, too, so all threads can compute on the prepared 
 * data.
 *
 * TODO: @todo Decide if to add a function to trywait if a barrier would be
 *             left by waiting on it. Do this via a register/unregister
 *             trywait functionality so the barrier can internally count if
 *             the already waiting threads and the one who registerd for
 *             trywait would be enought to resolve and leave the barrier.
 *
 * TODO: @todo The moment hardware supports it add a barrier type that is only
 *             valid for a group of cores - think OpenCL workgroup barriers.
 */

#ifndef AMP_amp_barrier_H
# define AMP_amp_barrier_H


#include <stddef.h>

#include <amp/amp_memory.h>



#if defined(__cplusplus)
extern "C" {
#endif


    
#define AMP_BARRIER_UNINITIALIZED NULL
    
    /**
     * Amp barrier type. Treat as opaque.
     */
    typedef struct amp_raw_barrier_s *amp_barrier_t;
    
    
    /**
     * After the number of threads a barrier waits for reached the barrier
     * it unblocks all of them and returns AMP_SUCCESS while exactly one thread
     * receives the value AMP_BARRIER_SERIAL_THREAD.
     *
     * This thread could then do some preparation work while all others
     * wait on the next barrier that guards the start of the next parallel
     * computations done by all threads. 
     */
#define AMP_BARRIER_SERIAL_THREAD ((int)-1)
    
    /**
     * Type of the barrier counter.
     */
    typedef unsigned int amp_barrier_count_t;
    
    
    
    /**
     * Creates and initializes barrier setting its counter to init_count.
     *
     * The platfrom backend functions to create the internals of the barrier
     * might not go through the allocator provided but might call malloc
     * directly.
     *
     * init_count must be greater than 0.
     *
     * If the initialization fails the allocator is called to free the
     * already allocated memory which must not result in an error or otherwise
     * behavior is undefined.
     *
     * @return AMP_SUCCESS on successful creation and initialization of barrier.
     *         AMP_NOMEM is returned if not enough memory is avaible.
     *         AMP_UNSUPPORTED is returned if barriers aren't supported.
     *         Other error codes might be returned to signal errors while
     *         creating, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR is returned if arguments are invalid or if not enough 
     *         platform resources exist to create the barrier internals.
     */
    int amp_barrier_create(amp_barrier_t* barrier,
                           amp_allocator_t allocator,
                           amp_barrier_count_t init_count);
    
    
    /**
     * Finalizes and frees barrier.
     *
     * Do not call if barrier is in use, e.g. if threads are waiting on it.
     *
     * The platfrom backend functions to destroy the internals of the barrier
     * might not go through the allocator provided but might call free
     * directly.
     *
     * allocator_context and dealloc_func must be capable of freeing the memory
     * allocated by the create function, otherwise behavior is undefined,
     * resources might be leaked, and barrier might end in an invalid and 
     * unrescueable state.
     *
     * @return AMP_SUCCESS on successful finalization and freeing of barrier.
     *         Error codes might be returned to signal 
     *         errors, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR is returned if barrier or allocator are NULL or if 
     *         threads are waiting on the barrier.
     *             
     */
    int amp_barrier_destroy(amp_barrier_t* barrier,
                            amp_allocator_t allocator);
    
    /**
     * Blocks a thread to wait on the barrier till as many threads are waiting
     * as was specified by init_count when creating the barrier.
     *
     * @return AMP_SUCCESS if enough threads wait on the barrier so all are
     *         woken up and pass it.
     *         AMP_BARRIER_SERIAL_THREAD is returned to exactly one thread.
     *         This thread could then do some preparation work while all others
     *         wait on the next barrier that guards the start of the next 
     *         parallel computations done by all threads.
     *         Error codes might be returned to signal 
     *         errors, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR might be returned if barrier is not valid.
     */
    int amp_barrier_wait(amp_barrier_t barrier);
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif


#endif /* AMP_amp_barrier_H */

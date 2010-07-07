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
 * Group of amp threads to launch and join with all contained threads without
 * the need to handle individual threads. After creation all threads of the
 * thread array need to be configured via the configure functions before the
 * thread array's threads can be all launched.
 *
 * Never pass an invalid, e.g. non-created thread array to any of the thread
 * array functions other than the create functions.
 *
 * Creation and destruction of threads, and especially a thread array is 
 * expensive and shouldn't be done in a tight loop. The amp_thread_array is 
 * targeted at simplifying writing of unit and stress tests that need groups of 
 * threads and for building thread or job pools that keep their threads alive 
 * for a long time.
 */
#ifndef AMP_amp_thread_array_H
#define AMP_amp_thread_array_H


#include <stddef.h>

#include <amp/amp_memory.h>
#include <amp/amp_thread.h>


#if defined(__cplusplus)
extern "C" {
#endif
    
    
#define AMP_THREAD_ARRAY_UNINITIALIZED NULL
    
    /**
     * Opaque type representing an amp thread array.
     */
    typedef struct amp_thread_array_s *amp_thread_array_t;

    
    /**
     * Allocates memory and creates a thread array that needs to be configured
     * before launching it.
     *
     * Behavior is undefined if thread_array represents an already 
     * (non-destroyed) created thread array.
     *
     * If the initialization fails the allocator is called to free the
     * already allocated memory which must not result in an error or otherwise
     * behavior is undefined.
     *
     * @return AMP_SUCCESS on successful creation of thread_group. On error the
     *         following error codes are returned:
     *         AMP_NOMEM if not enough memory is available.
     *         Other error codes might be returned to signal 
     *         errors, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR is returned if thread_array, thread_count, or
     *         allocator are NULL.
     *         On error no memory is leaked.
     *         
     * TODO: @todo Decide if to allow @c 0 as a thread count.
     */
    int amp_thread_array_create(amp_thread_array_t* thread_array,
                                amp_allocator_t allocator,
                                size_t thread_count);
    
    
    /**
     * Finalizes the thread array and frees its memory.
     *
     * If any thread array's threads have been launched they must be 
     * joined before destroying the array. It isn't done automatically
     * because joining can block and the amp user must know what she is
     * doing and explicitly decide how to handle shutdown.
     *
     * allocator_context and dealloc_func must be able to free the memory 
     * allocated by the allocator_context and alloc_func arguments used when 
     * calling amp_thread_array_create in thread_array otherwise behavior is
     * undefined. Error codes might be returned but the thread_array might be
     * in an invalid and non-rescueable state. Memory might be leaked.
     *
     * @return AMP_SUCCESS is returned on successful finalization of the thread
     *         array and after its memory is freed. On error the following error
     *         codes might be returned:
     *         Other error codes might be returned to signal 
     *         errors, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_BUSY is returned if not all launched threads of the array 
     *         have been joined.
     *         AMP_ERROR  is returned if thread_array or allocator are NULL.
     */
    int amp_thread_array_destroy(amp_thread_array_t* thread_array,
                                 amp_allocator_t allocator);
    
    /**
     * Sets range_length thread array thread contexts starting at index 
     * range_begin to shared_context.
     *
     * If an error is returned it is not defined if or which of the thread
     * contexts of the array have been changed.
     *
     * range_length must not be greater than the size of thread_array.
     * range_length must not be 0.
     * All indices from range_begin inside range_length must be inside the 
     * index range of thread_array.
     *
     * Do not call after launching and before joining with a thread array.
     *
     * @return AMP_SUCCESS on successful configuration.
     *         Other error codes might be returned to signal errors, too. These
     *         are programming errors and must not occur in release code. When
     *         @em amp is compiled without NDEBUG set it might assert that these
     *         errors do not happen.
     *         AMP_ERROR might be returned if the arguments are invalid.
     *         AMP_BUSY might be returned if the thread array is already 
     *         launched.
     */
    int amp_thread_array_configure_contexts(amp_thread_array_t thread_array,
                                            size_t range_begin,
                                            size_t range_length,
                                            void* shared_context);
    
    /**
     * Sets range_length thread array thread functions starting at index 
     * range_begin to shared_function.
     *
     * If an error is returned it us not defined if or which of the thread
     * functions of the array have been changed.
     *
     * range_length must not be greater than the size of thread_array.
     * range_length must not be 0.
     * All indices from range_begin inside range_length must be inside the 
     * index range of thread_array.
     * shared_function must not be NULL or invalid.
     *
     * Do not call after launching and before joining with a thread array.
     *
     * @return AMP_SUCCESS on successful configuration.
     *         Other error codes might be returned to signal errors, too. These
     *         are programming errors and must not occur in release code. When
     *         @em amp is compiled without NDEBUG set it might assert that these
     *         errors do not happen.
     *         AMP_ERROR might be returned if the arguments are invalid.
     *         AMP_BUSY might be returned if the thread array is already 
     *         launched.
     */
    int amp_thread_array_configure_functions(amp_thread_array_t thread_array,
                                             size_t range_begin,
                                             size_t range_length,
                                             amp_thread_func_t shared_function);
    
    /**
     * Sets range_length thread array thread contexts and functions starting at 
     * index range_begin to shared_context and shared_function.
     *
     * If an error is returned it us not defined if or which of the thread
     * contexts or functions of the array have been changed.
     *
     * range_length must not be greater than the size of thread_array.
     * range_length must not be 0.
     * All indices from range_begin inside range_length must be inside the 
     * index range of thread_array.
     * shared_function must not be NULL or invalid.
     *
     * Do not call after launching and before joining with a thread array.
     *
     * @return AMP_SUCCESS on successful configuration.
     *         Other error codes might be returned to signal errors, too. These
     *         are programming errors and must not occur in release code. When
     *         @em amp is compiled without NDEBUG set it might assert that these
     *         errors do not happen.
     *         AMP_ERROR might be returned if the arguments are invalid.
     *         AMP_BUSY might be returned if the thread array is already 
     *         launched.
     */
    int amp_thread_array_configure(amp_thread_array_t thread_array,
                                   size_t range_begin,
                                   size_t range_length,
                                   void* shared_context,
                                   amp_thread_func_t shared_function);
    
    
    /**
     * Launches the contained threads one after the other and stops if
     * thread launching fails. The number of threads launched is returned
     * in joinable if the pointer is not NULL.
     * If not all threads could be launched nothing is done other then
     * returning AMP_ERROR or AMP_NOMEM and the threads that could be launched
     * are running or are about to run. 
     *
     * Call join_all to join with the launched threads.
     *
     * You can handle non-launcheable threads of a thread array in at least
     * three ways:
     * <ol>
     *    <li> Terminate the app if the number of launched threads doesn't
     *         equal the number of threads the thread array should have 
     *         launched. Call join all on the thread array if you want to 
     *         end the application in a coordinated way.
     *    </li>
     *    <li> Order the thread contexts and functions so that even if not 
     *         all threads of the array could be launched the launched
     *         threads are able to work with each other and don't need the
     *         non-launched threads (functions).
     *    </li>
     *    <li> Re-run launch as often until joinable indicates that
     *         all threads of the thread array have been launched and
     *         can therefore be joined.
     *         Be careful with join all counts between launch all calls as
     *         launch all will try to launch non-launched and also already 
     *         joined threads.
     *    </li>
     * </ol>
     * In all cases it makes sense to code the thread functions so the
     * launch process runs through two stages (launch threads into "orbit"
     * and after all necessary thread functions signaled that they are in 
     * orbit let them start their work). Each thread function uses a 
     * condition variable to signal that it wants to proceed after it has
     * been launched. The condition variable predicate can then signal 
     * that the function should proceed or that it should shut down, e.g. if
     * not all necessary threads could have been launched.
     *
     * @return AMP_SUCCESS on successful launch of all launcheable threads of
     *         the thread array.
     *         AMP_ERROR is returned if the system lacks the resources for 
     *         thread creation and launching.
     *         Other error codes might be returned to signal 
     *         errors, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     */
    int amp_thread_array_launch_all(amp_thread_array_t thread_array,
                                    size_t* joinable_thread_count);
    
    
    
    /**
     * Joins with all joinable threads of the thread array.
     *
     * Threads are launched from left to right and joined from right to left.
     * On the first thread that can't be joined the joining stops and the 
     * error code of the internal amp_raw_thread_join function is returned. If
     * joinable_thread_count isn't NULL the number of remaining un-joined
     * threads "left" from the non-joinable thread is returned in it.
     *
     * @return AMP_SUCCESS if all joinable threads have been joined with.
     *         Other error codes might be returned to signal 
     *         errors, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR might be returned by the internally called
     *         amp_raw_thread_join on error.
     */
    int amp_thread_array_join_all(amp_thread_array_t thread_array,
                                  size_t* joinable_thread_count);
    
    
    /**
     * Returns the number of joinable threads in the variable 
     * joinable_thread_count points to.
     *
     * @return AMP_SUCCESS if the joinable thread count could be determined
     *         successfully. Other error codes might be returned to signal 
     *         errors, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     */
    int amp_thread_array_get_joinable_thread_count(amp_thread_array_t thread_array,
                                                   size_t* joinable_thread_count);
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif


#endif /* AMP_amp_thread_array_H */

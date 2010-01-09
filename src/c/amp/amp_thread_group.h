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
 * Group of amp threads to launch and join with all contained threads without
 * the need to handle individual threads.
 *
 * Never pass an invalid, e.g. non-created thread group to any of the thread
 * group functions other than the create functions.
 */
#ifndef AMP_amp_thread_group_H
#define AMP_amp_thread_group_H


#include <stddef.h>

/* TODO: @todo Replace with amp_thread.h the moment it exists. */
#include <amp/amp_raw_thread.h>
#include <amp/amp_memory.h>
#include <amp/amp_stddef.h>



#if defined(__cplusplus)
extern "C" {
#endif
    
    /**
     * Opaque type representing an amp thread group.
     */
    typedef struct amp_thread_group_s *amp_thread_group_t;
    
    /**
     * Context for the amp_thread_group functions to access user specifyable 
     * alloc and dealloc functionality, e.g. get memory from a specific memory 
     * pool.
     */
    struct amp_thread_group_context_s {
        amp_alloc_func_t alloc_func;
        amp_dealloc_func_t dealloc_func;
        void *allocator_context;
    };
    
    
    
    /**
     * Function interface to implement by the user which takes an also user
     * supplied context pointer or enumerator pointer and then returns an
     * available item and advances to the next item or returns NULL and doesn't
     * advance if no further items can be enumerated in the enumerator.
     *
     * See amp_raw_byte_range_next as an example.
     */
    typedef void* (*amp_enumerator_next_func_t)(void *enumerator);
    
    /**
     * Function interface to implement by the user which takes an also user
     * supplied context pointer or enumerator pointer and then returns an
     * available amp_thread_func_t function and advances to the next function or 
     * returns NULL and doesn't advance if no further functions can be 
     * enumerated in the enumerator.
     *
     * See amp_raw_byte_range_next_func as an example.
     */
    typedef amp_thread_func_t (*amp_enumerator_next_func_func_t)(void *enumerator);
    
    
    
    /**
     *
     * Doesn't take over ownership of group_context, thread_contexts, or
     * thread_functions. However, the group context and the items inside
     * thread_contexts and thread_functions (but not the ranges itself) are 
     * used throughout the lifetime of the thread group and mustn't be
     * destroyed until the thread group has been destroyed.
     * 
     * thread_context_enumerator is a user supplied data structure which can
     * be used in combination with enumerator_func to iteratre over thread
     * context data at least thread_count times. The accessed thread contexts
     * are associated with threads from the thread group.
     * See amp_thread_group_test.cpp for a usage example and amp_raw_byte_range
     * for an example of an enumerator type.
     *
     * thread_func_enumerator is a user supplied data structure which can be
     * used in combination with func_enumerator_func to iterate over thread
     * functions at least thread_count times. The accessed thread functions are
     * associated with the threads from the thread group.
     * See amp_thread_group_test.cpp for a usage example and amp_raw_byte_range
     * for an example of an enumerator type.
     *
     * No entry of thread_functions must be NULL, otherwise behavior is 
     * undefined.
     *
     * Behavior is undefined if thread_group represents an already 
     * (non-destroyed) created thread group.
     *
     * @return AMP_SUCCESS on successful creation of thread_group. On error the
     *         following error codes are returned:
     *         ENOMEM if not enough memory is available.
     *         Other error codes might be returned to signal 
     *         errors, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         EINVAL is returned if thread_group, group_context, 
     *         thread_context_enumerator_func, or thread_func_enumerator_func
     *         are NULL.
     *         EINVAL is also returned if the function pointers of group_context
     *         are NULL.
     *         EINVAL is returned if thread_func_enumerator_func returns NULL.
     *         
     * TODO: @todo Decide if to allow @c 0 as a thread count.
     */
    int amp_thread_group_create(amp_thread_group_t *thread_group,
                                struct amp_thread_group_context_s *group_context,
                                size_t thread_count,
                                void *thread_context_enumerator,
                                amp_enumerator_next_func_t thread_context_enumerator_func,
                                void *thread_func_enumerator,
                                amp_enumerator_next_func_func_t thread_func_enumerator_func);
    
    
    /**
     * Creates a thread group like amp_thread_group_create but uses a single
     * function to run on each thread of the thread group.
     *
     * @return AMP_SUCCESS on successful creation of thread_group. On error the
     *         following error codes are returned:
     *         ENOMEM if not enough memory is available.
     *         Other error codes might be returned to signal 
     *         errors, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         EINVAL is returned if thread_group, group_context, 
     *         thread_context_enumerator_func, or thread_function
     *         are NULL - which is considered a programmer error.
     *         EINVAL is also returned if the function pointers of group_context
     *         are NULL.
     */
    int amp_thread_group_create_with_single_func(amp_thread_group_t *thread_group,
                                                 struct amp_thread_group_context_s *group_context,
                                                 size_t thread_count,
                                                 void *thread_context_enumerator,
                                                 amp_enumerator_next_func_t thread_context_enumerator_func,
                                                 amp_thread_func_t thread_function);
    
    
    
    /**
     * Finalizes the thread group and frees its memory using the services 
     * provided by group_context.
     *
     * If any thread groups threads have been launched they must be 
     * joined before destroying the group. It isn't done automatically
     * because joining can block and the amp user must know what she is
     * doing and explicitly decide how to handle shutdown.
     *
     * @return AMP_SUCCESS is returned on successful finalization of the thread
     *         group and after its memory is freed. On error the following error
     *         codes might be returned:
     *         EBUSY is returned if not all launched threads of the group have
     *         been joined.
     *         Other error codes might be returned to signal 
     *         errors, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         EINVAL is returned if thread_group, group_context, or the
     *         function pointers in group_context are NULL.
     */
    int amp_thread_group_destroy(amp_thread_group_t thread_group,
                                 struct amp_thread_group_context_s *group_context);
    
    
    
    /**
     * Launches the contained threads one after the other and stops if
     * thread launching fails. The number of threads launched is returned
     * in joinable if the pointer is not NULL.
     * If not all threads could be launched nothing is done other then
     * returning EAGAIN or ENOMEM and the threads that could be launched
     * are running and not joined with. 
     * Call join all to join with the launched threads after the thread
     * functions have exited.
     *
     * You can handle non-launcheable threads of a thread group in at least
     * three ways:
     * <ol>
     *    <li> Terminate the app if the number of launched threads doesn't
     *         equal the number of threads the thread group should have 
     *         launched. Call join all on the thread group if you want to 
     *         end the application in a coordinated way.
     *    </li>
     *    <li> Order the thread contexts and functions so that even if not 
     *         all threads of the group could be launched the launched
     *         threads are able to work with each other and don't need the
     *         non-launched threads (functions).
     *    </li>
     *    <li> Re-run launch as often unitl joinable indicates that
     *         all threads of the thread group have been launched and
     *         can therefore be joined.
     *         Be careful with join all counts between launch all calls as
     *         launch all will try to launch non-launched and also already 
     *         joined threads.
     *    </li>
     * </ol>
     * In all cases it makes sense to code the thread functions so the
     * launch process runs through two stages (launch threads into "orbit"
     * and after all necessary thread functions signalled that they are in 
     * orbit let them start their work). Each thread function uses a 
     * condition variable to signal that it wants to proceed after it has
     * been launched. The condition variable predicate can then signal 
     * that the function should proceed or that it should shut down, e.g. if
     * not all necessary threads could have been launched.
     *
     * @return AMP_SUCCESS on successful launch of all launcheable threads of
     *         the thread group.
     *         EAGAIN is returned if the system lacks the resources for thread
     *         creation and launching.
     *         Other error codes might be returned to signal 
     *         errors, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         EINVAL is returned if the thread_group is NULL or invalid.
     */
    int amp_thread_group_launch_all(amp_thread_group_t thread_group,
                                    size_t *joinable_thread_count);
    
    
    
    /**
     * Joins with all joinable threads of the thread group.
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
     *         EINVAL is returned if thread_group is NULL.
     *         EDEADLK, EINVAL, ESRCH might be returned by the internally called
     *         amp_raw_thread_join on error.
     */
    int amp_thread_group_join_all(amp_thread_group_t thread_group,
                                  size_t *joinable_thread_count);
    
    
    /**
     * Returns the number of joinable threads in the variable 
     * joinable_thread_count points to.
     *
     * @return AMP_SUCCESS if the joinable thread count could be determined
     *         successfully. Other error codes might be returned to signal 
     *         errors, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         EINVAL is returned if thread_group or joinable_thread_count are 
     *         NULL.
     */
    int amp_thread_group_get_joinable_thread_count(amp_thread_group_t thread_group,
                                                   size_t *joinable_thread_count);
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif


#endif /* AMP_amp_thread_group_H */

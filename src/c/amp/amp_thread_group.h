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
 * TODO: @todo Don't store the functions and context - amp thread is already
 *             doing this and memory usage should be minimized.
 *
 * TODO: @todo Decide if to store the context pointer or to require that the
 *             user provides it for destroying the thread group again.
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
     * TODO: @todo Add error handling.
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
     *
     *
     * thread_functions mustn't be NULL, otherwise behavior is undefined.
     */
    int amp_thread_group_create_with_single_func(amp_thread_group_t *thread_group,
                                                 struct amp_thread_group_context_s *group_context,
                                                 size_t thread_count,
                                                 void *thread_context_enumerator,
                                                 amp_enumerator_next_func_t thread_context_enumerator_func,
                                                 amp_thread_func_t thread_function);
    
    
    
    /**
     *
     * If any thread groups threads have been launched they must be 
     * joined before destroying the group. It isn't done automatically
     * because joining can block and the amp user must know what she is
     * doing and explicitly decide how to handle shutdown.
     */
    int amp_thread_group_destroy(amp_thread_group_t thread_group,
                                 struct amp_thread_group_context_s *group_context);
    
    
    
    /**
     * 
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
     *         non-launched thread (functions).
     *    </li>
     *    <li> Re-run launch all as long until joinable indicates that
     *         all threads of the thread group have been launched and
     *         can therefore be joined.
     *         Be careful with join all counts between launch all calls as
     *         launch all will try to launch non-launched and already joined 
     *         threads.
     *    </li>
     * </ol>
     * In both cases it makes sense to code the thread functions so the
     * launch process runs through two stages (launch threads into "orbit"
     * and after all necessary thread functions signalled that they are in 
     * orbit let them start their work). Each thread function uses a 
     * condition variable to signal that it wants to proceed after it has
     * been launched. The condition variable predicate can then signal 
     * that the function should proceed or that it should shut down, e.g. if
     * not all necessary threads could have been launched.
     */
    int amp_thread_group_launch_all(amp_thread_group_t thread_group,
                                    size_t *joinable_thread_count);
    
    
    
    /**
     *
     * If joinable_thread_count isn't NULL the number of remaining joinable
     * threads is assigned to it. If it isn't 0 then only the thread joins
     * down from the joinable threads before to before first non-joinable thread
     * have been joined.
     *
     * Threads are launched from left to right and joined from right to left.
     */
    int amp_thread_group_join_all(amp_thread_group_t thread_group,
                                  size_t *joinable_thread_count);
    
    
    
    /**
     * Returns the group context.
     *
     * Remember, the group context memory isn't managed by the thread group and
     * the context memory isn't freed on destroying the thread group.
     *
     * TODO: @todo Decide if really necessary.
     */
    /*
    int amp_thread_group_get_context(amp_thread_group_t thread_group, 
                                     struct amp_thread_group_context_s **context);
    */
    
    /**
     *
     * TODO: @todo Unit test amp_thread_group_get_joinable_thread_count.
     */
    int amp_thread_group_get_joinable_thread_count(amp_thread_group_t thread_group,
                                                   size_t *joinable_thread_count);
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif


#endif /* AMP_amp_thread_group_H */

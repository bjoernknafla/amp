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
 * Implementation of amp thread group.
 */

#include "amp_thread_group.h"

#include <assert.h>
#include <errno.h>


#include "amp_stddef.h"
#include "amp_internal_raw_thread.h"





struct amp_thread_group_s {
    struct amp_raw_thread_s *threads;
    size_t thread_count;
    size_t joinable_count;
    /* struct amp_thread_group_context_s *context;*/
};



/**
 * Allocates the necessary memory and stores context and threac count info
 * in the thread group but doesn't initialize the threads for launching.
 */
static int amp_internal_thread_group_create(struct amp_thread_group_s **thread_group,
                                            struct amp_thread_group_context_s *group_context,
                                            size_t thread_count)
{
    /* TODO: @todo Decide if only to allocate one big area of memory to put
     *             the group thread structs and the group itself into.
     */
    
    assert(NULL != thread_group);
    assert(NULL != group_context);
    assert(NULL != group_context->alloc_func);
    assert(NULL != group_context->dealloc_func);
    
    if ((NULL == thread_group) 
        || (NULL == group_context)) {
        
        return EINVAL;
    }

    
    amp_alloc_func_t alloc = group_context->alloc_func;
    amp_dealloc_func_t dealloc = group_context->dealloc_func;
    void *allocator_context = group_context->allocator_context;
    
    *thread_group = NULL;
    
    struct amp_thread_group_s *group =  
        (struct amp_thread_group_s *)alloc(allocator_context,
                                           sizeof(struct amp_thread_group_s));
    assert(NULL != group);
    if (NULL == group) {
        return ENOMEM;
    }
    
    /* Allocate memory for the groups threads. */
    struct amp_raw_thread_s* threads = 
        (struct amp_raw_thread_s*)alloc(allocator_context,
                                        sizeof(struct amp_raw_thread_s) * thread_count);
    assert(NULL != threads);
    if (NULL == threads) {
        
        dealloc(allocator_context, group);
        
        return ENOMEM;
    }
    
    
    group->threads = threads;
    group->thread_count = thread_count;
    group->joinable_count = 0;
    /* group->context = group_context; */
    
    *thread_group = group;
    
    return AMP_SUCCESS;    
}



int amp_thread_group_create(amp_thread_group_t *thread_group,
                            struct amp_thread_group_context_s *group_context,
                            size_t thread_count,
                            void *thread_context_enumerator,
                            amp_enumerator_next_func_t thread_context_enumerator_func,
                            void *thread_func_enumerator,
                            amp_enumerator_next_func_func_t thread_func_enumerator_func)
{
    assert(NULL != thread_context_enumerator_func);
    assert(NULL != thread_func_enumerator_func);
    
    if (NULL == thread_context_enumerator_func 
        || NULL == thread_func_enumerator_func) {
        return EINVAL;
    }
    
    int retval = amp_internal_thread_group_create(thread_group,
                                                  group_context,
                                                  thread_count);
    
    if (AMP_SUCCESS == retval) {
        
        struct amp_thread_group_s *group = *thread_group;
        struct amp_raw_thread_s *threads = group->threads;
        
        for (size_t i = 0; i < thread_count; ++i) {
            
            void *thread_context = thread_context_enumerator_func(thread_context_enumerator);
            amp_thread_func_t thread_func = thread_func_enumerator_func(thread_func_enumerator);
            
            assert(NULL != thread_func && "Thread functions must not be NULL.");
            
            int const rv = amp_internal_raw_thread_init(&threads[i], 
                                                        thread_context, 
                                                        thread_func);
            assert(AMP_SUCCESS == rv);
            if (AMP_SUCCESS != rv) {
                
                int const rvd = amp_thread_group_destroy(*thread_group,
                                                         group_context);
                *thread_group = NULL;
                assert(AMP_SUCCESS == rvd);
                
                return rv;
            }
        }
    }
    

    return retval;
}



int amp_thread_group_create_with_single_func(amp_thread_group_t *thread_group,
                                             struct amp_thread_group_context_s *group_context,
                                             size_t thread_count,
                                             void *thread_context_enumerator,
                                             amp_enumerator_next_func_t thread_context_enumerator_func,
                                             amp_thread_func_t thread_function)
{
    assert(NULL != thread_context_enumerator_func);
    assert(NULL != thread_function);
    
    if (NULL == thread_context_enumerator_func
        || NULL == thread_function) {
        return EINVAL;
    }
    
    int retval = amp_internal_thread_group_create(thread_group,
                                                       group_context,
                                                       thread_count);
    
    if (AMP_SUCCESS == retval) {
        
        struct amp_thread_group_s *group = *thread_group;
        struct amp_raw_thread_s *threads = group->threads;
        
        for (size_t i = 0; i < thread_count; ++i) {
            
            void *thread_context = thread_context_enumerator_func(thread_context_enumerator);
            
            int const rv = amp_internal_raw_thread_init(&threads[i], 
                                                        thread_context, 
                                                        thread_function);
            assert(AMP_SUCCESS == rv);
            if (AMP_SUCCESS != rv) {
                
                int const rvd = amp_thread_group_destroy(*thread_group,
                                                         group_context);
                *thread_group = NULL;
                assert(AMP_SUCCESS == rvd);
                
                return rv;
            }
        }
    }
    
    
    return retval;    
}



int amp_thread_group_destroy(struct amp_thread_group_s *thread_group,
                             struct amp_thread_group_context_s *thread_group_context)
{
    assert(NULL != thread_group);
    assert(NULL != thread_group_context);
    assert(0 == thread_group->joinable_count);
    
    if (NULL == thread_group || NULL == thread_group_context) {
        return EINVAL;
    }
    
    if ((0 != thread_group->joinable_count)) {
        return EBUSY;
    }
    
    amp_dealloc_func_t dealloc = thread_group_context->dealloc_func;
    void *allocator_context = thread_group_context->allocator_context;
    
    dealloc(allocator_context, thread_group->threads);
    dealloc(allocator_context, thread_group);
    
    return AMP_SUCCESS;
}


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
int amp_thread_group_launch_all(struct amp_thread_group_s *thread_group,
                                size_t *joinable_thread_count)
{
    assert(NULL != thread_group);
    
    if (NULL == thread_group) {
        return EINVAL;
    }
    
    struct amp_raw_thread_s *threads = thread_group->threads;
    size_t joinable_count = thread_group->joinable_count;
    size_t const thread_count = thread_group->thread_count;
    
    int retval = AMP_SUCCESS;
    while (   (joinable_count < thread_count)
           && (AMP_SUCCESS == retval)) {
        
        retval = amp_internal_raw_thread_launch_initialized(&(threads[joinable_count]));
        
        if (AMP_SUCCESS == retval) {
            ++joinable_count;
        }
    }
    
    thread_group->joinable_count = joinable_count;
    
    if (NULL != joinable_thread_count) {
        *joinable_thread_count = joinable_count;
    }
    
    return retval;
}


int amp_thread_group_join_all(struct amp_thread_group_s *thread_group,
                              size_t *joinable_thread_count)
{
    assert(NULL != thread_group);
    
    if (NULL == thread_group) {
        return EINVAL;
    }
    
    struct amp_raw_thread_s *threads = thread_group->threads;
    size_t const joinable_count = thread_group->joinable_count;
    size_t joined_count = 0;
    
    int retval = AMP_SUCCESS;
    while (   (joined_count < joinable_count)
           && (AMP_SUCCESS == retval)) {
        
        /* Launching from left to right, joining from right to left. */
        retval = amp_raw_thread_join(&(threads[joinable_count - 1 - joined_count]));
        
        if (AMP_SUCCESS == retval) {
            ++joined_count;    
        }
    }
    
    thread_group->joinable_count -= joined_count;
    
    
    if (NULL != joinable_thread_count) {
        *joinable_thread_count = thread_group->joinable_count;
    }
    
    return retval;
}


/* TODO: @todo Remove old and dead code.
int amp_thread_group_get_context(struct amp_thread_group_s *thread_group, 
                                 struct amp_thread_group_context_s **context)
{
    assert(NULL != thread_group);
    assert(NULL != context);
    
    if (NULL == thread_group || NULL == context) {
        return EINVAL;
    }
    
    *context = thread_group->context;
    
    return AMP_SUCCESS;
}
*/


int amp_thread_group_get_joinable_thread_count(amp_thread_group_t thread_group,
                                               size_t *joinable_thread_count)
{
    assert(NULL != thread_group);
    assert(NULL != joinable_thread_count);

    if (NULL == thread_group || NULL == joinable_thread_count) {
        return EINVAL;
    }
    
    *joinable_thread_count = thread_group->joinable_count;
    
    return AMP_SUCCESS;
}



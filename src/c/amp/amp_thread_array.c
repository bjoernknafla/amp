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
 * Implementation of amp thread array.
 */

#include "amp_thread_array.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include "amp_stddef.h"
#include "amp_thread.h"
#include "amp_raw_thread.h"
#include "amp_internal_thread.h"



struct amp_thread_array_s {
    struct amp_raw_thread_s *threads;
    size_t thread_count;
    size_t joinable_count;
    /* struct amp_thread_array_context_s *context;*/
};



int amp_thread_array_create(amp_thread_array_t *thread_array,
                            size_t thread_count,
                            void *allocator_context,
                            amp_alloc_func_t alloc_func,
                            amp_dealloc_func_t dealloc_func)
{
    /* TODO: @todo Decide if only to allocate one big area of memory to put
     *             the group thread structs and the group itself into.
     */
    
    assert(NULL != thread_array);
    assert(0 != thread_count);
    assert(NULL != alloc_func);
    assert(NULL != dealloc_func);
    
    if (NULL == thread_array
        || 0 == thread_count
        || NULL == alloc_func
        || NULL == dealloc_func) {
        
        return EINVAL;
    }
    
    *thread_array = NULL;
    
    struct amp_thread_array_s *group =  
    (struct amp_thread_array_s *)alloc_func(allocator_context,
                                            sizeof(struct amp_thread_array_s));
    if (NULL == group) {
        return ENOMEM;
    }
    
    /* Allocate memory for the groups threads. */
    struct amp_raw_thread_s *threads = 
    (struct amp_raw_thread_s *)alloc_func(allocator_context,
                                         sizeof(struct amp_raw_thread_s) * thread_count);
    if (NULL == threads) {
        
        dealloc_func(allocator_context, group);
        
        return ENOMEM;
    }
    
    for (size_t i = 0; i < thread_count; ++i) {
        
        int rv = amp_internal_thread_init_for_configuration(&threads[i]);
        assert(AMP_SUCCESS == rv);
    }
    
    
    group->threads = threads;
    group->thread_count = thread_count;
    group->joinable_count = (size_t)0;
    
    *thread_array = group;
    
    return AMP_SUCCESS;   
}



int amp_thread_array_destroy(amp_thread_array_t thread_array,
                             void *allocator_context,
                             amp_dealloc_func_t dealloc_func)
{
    assert(NULL != thread_array);
    assert(NULL != dealloc_func);
    assert(0 == thread_array->joinable_count);
    
    if (NULL == thread_array 
        || NULL == dealloc_func) {
        
        return EINVAL;
    }
    
    if ((0 != thread_array->joinable_count)) {
        return EBUSY;
    }
    
    dealloc_func(allocator_context, thread_array->threads);
    dealloc_func(allocator_context, thread_array);
    
    return AMP_SUCCESS;
}



int amp_thread_array_configure_contexts(amp_thread_array_t thread_array,
                                        size_t index_begin,
                                        size_t index_end, /* exclusive */
                                        void *shared_context)
{
    assert(NULL != thread_array);
    assert(index_begin < thread_array->thread_count);
    assert(index_end <= thread_array->thread_count);
    assert(0 == thread_array->joinable_count);
    
    size_t const thread_count = thread_array->thread_count;
    
    if (NULL == thread_array
        || index_begin >= thread_count
        || index_end > thread_count) {
        
        return EINVAL;
    }
    if (0 != thread_array->joinable_count) {
        return EBUSY;
    }
    
    struct amp_raw_thread_s *threads = thread_array->threads;
    
    for (size_t i = index_begin; i < index_end; ++i) {
        int const errc = amp_internal_thread_configure_context(&threads[i],
                                                               shared_context);
        if (AMP_SUCCESS != errc) {
            return errc;
        }
    }
    
    return AMP_SUCCESS;
}



int amp_thread_array_configure_functions(amp_thread_array_t thread_array,
                                         size_t index_begin,
                                         size_t index_end, /* exclusive */
                                         amp_thread_func_t shared_function)
{
    assert(NULL != thread_array);
    assert(index_begin < thread_array->thread_count);
    assert(index_end <= thread_array->thread_count);
    assert(0 == thread_array->joinable_count);
    
    size_t const thread_count = thread_array->thread_count;
    
    if (NULL == thread_array
        || index_begin >= thread_count
        || index_end > thread_count) {
        
        return EINVAL;
    }
    if (0 != thread_array->joinable_count) {
        return EBUSY;
    }
    
    struct amp_raw_thread_s *threads = thread_array->threads;
    
    for (size_t i = index_begin; i < index_end; ++i) {
        int const errc = amp_internal_thread_configure_function(&threads[i],
                                                                shared_function);
        if (AMP_SUCCESS != errc) {
            return errc;
        }
    }
    
    return AMP_SUCCESS;
}



int amp_thread_array_configure(amp_thread_array_t thread_array,
                               size_t index_begin,
                               size_t index_end, /* exclusive */
                               void *shared_context,
                               amp_thread_func_t shared_function)
{
    assert(NULL != thread_array);
    assert(index_begin < thread_array->thread_count);
    assert(index_end <= thread_array->thread_count);
    assert(0 == thread_array->joinable_count);
    
    size_t const thread_count = thread_array->thread_count;
    
    if (NULL == thread_array
        || index_begin >= thread_count
        || index_end > thread_count) {
        
        return EINVAL;
    }
    if (0 != thread_array->joinable_count) {
        return EBUSY;
    }
    
    struct amp_raw_thread_s *threads = thread_array->threads;
    
    for (size_t i = index_begin; i < index_end; ++i) {
        int const errc0 = amp_internal_thread_configure_context(&threads[i],
                                                                shared_context);
        int const errc1 = amp_internal_thread_configure_function(&threads[i],
                                                                 shared_function);
        if (AMP_SUCCESS != errc0
            || AMP_SUCCESS != errc1) {
            
            return (AMP_SUCCESS != errc0 ? errc0 : errc1);
        }
    }
    
    return AMP_SUCCESS;
}


int amp_thread_array_launch_all(struct amp_thread_array_s *thread_array,
                                size_t *joinable_thread_count)
{
    assert(NULL != thread_array);
    
    if (NULL == thread_array) {
        return EINVAL;
    }
    
    struct amp_raw_thread_s *threads = thread_array->threads;
    size_t joinable_count = thread_array->joinable_count;
    size_t const thread_count = thread_array->thread_count;
    
    int retval = AMP_SUCCESS;
    while (   (joinable_count < thread_count)
           && (AMP_SUCCESS == retval)) {
        
        retval = amp_internal_thread_launch_configured(&(threads[joinable_count]));
        
        if (AMP_SUCCESS != retval) {
            break;
        }
        
        ++joinable_count;
    }
    
    thread_array->joinable_count = joinable_count;
    
    if (NULL != joinable_thread_count) {
        *joinable_thread_count = joinable_count;
    }
    
    return retval;
}


int amp_thread_array_join_all(struct amp_thread_array_s *thread_array,
                              size_t *joinable_thread_count)
{
    assert(NULL != thread_array);
    
    if (NULL == thread_array) {
        return EINVAL;
    }
    
    struct amp_raw_thread_s *threads = thread_array->threads;
    size_t const joinable_count = thread_array->joinable_count;
    size_t joined_count = 0;
    
    int retval = AMP_SUCCESS;
    while (   (joined_count < joinable_count)
           && (AMP_SUCCESS == retval)) {
        
        /* Launching from left to right, joining from right to left. */
        retval = amp_raw_thread_join(&(threads[joinable_count - 1 - joined_count]));
        
        if (AMP_SUCCESS != retval) {
            break;
        }
        
        ++joined_count;
    }
    
    thread_array->joinable_count -= joined_count;
    
    
    if (NULL != joinable_thread_count) {
        *joinable_thread_count = thread_array->joinable_count;
    }
    
    return retval;
}



int amp_thread_array_get_joinable_thread_count(amp_thread_array_t thread_array,
                                               size_t *joinable_thread_count)
{
    assert(NULL != thread_array);
    assert(NULL != joinable_thread_count);

    if (NULL == thread_array || NULL == joinable_thread_count) {
        return EINVAL;
    }
    
    *joinable_thread_count = thread_array->joinable_count;
    
    return AMP_SUCCESS;
}



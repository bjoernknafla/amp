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
#include <stddef.h>

#include "amp_stddef.h"
#include "amp_return_code.h"
#include "amp_thread.h"
#include "amp_raw_thread.h"
#include "amp_internal_thread.h"


/** 
 * Internal opaque thread array data structure 
 */
struct amp_thread_array_s {
    struct amp_raw_thread_s *threads;
    size_t thread_count;
    size_t joinable_count;
    /* struct amp_thread_array_context_s *context;*/
};



int amp_thread_array_create(amp_thread_array_t* thread_array,
                            amp_allocator_t allocator,
                            size_t thread_count)
{
    /* TODO: @todo Decide if only to allocate one big area of memory to put
     *             the group thread structs and the group itself into.
     */
    
    struct amp_thread_array_s* group = NULL;
    struct amp_raw_thread_s* threads = NULL;
    size_t i = 0;

    assert(NULL != thread_array);
    assert(0 != thread_count);
    assert(NULL != allocator);
    
    *thread_array = AMP_THREAD_ARRAY_UNINITIALIZED;
    
    if ( 0 == thread_count) {
        
        return AMP_ERROR;
    }
    
    *thread_array = NULL;
    
    group = (struct amp_thread_array_s *)AMP_ALLOC(allocator, sizeof(*group));
    if (NULL == group) {
        return AMP_NOMEM;
    }
    
    /* Allocate memory for the groups threads. */
    threads = (struct amp_raw_thread_s*)AMP_CALLOC(allocator,
                                                   thread_count,
                                                   sizeof(*threads));
    if (NULL == threads) {
        int const rv = AMP_DEALLOC(allocator, group);
        assert(AMP_SUCCESS == rv);
        (void)rv;
        
        return AMP_NOMEM;
    }
    
    for (i = 0; i < thread_count; ++i) {
        int const rv = amp_internal_thread_init_for_configuration(&threads[i]);
        assert(AMP_SUCCESS == rv);
        (void)rv;
    }
    
    
    group->threads = threads;
    group->thread_count = thread_count;
    group->joinable_count = (size_t)0;
    
    *thread_array = group;
    
    return AMP_SUCCESS;   
}



int amp_thread_array_destroy(amp_thread_array_t* thread_array,
                             amp_allocator_t allocator)
{
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != thread_array);
    assert(NULL != *thread_array);
    assert(NULL != allocator);
    assert(0 == (*thread_array)->joinable_count);

    
    if ((0 != (*thread_array)->joinable_count)) {
        return AMP_BUSY;
    }
    
    retval = AMP_DEALLOC(allocator, (*thread_array)->threads);
    if (AMP_SUCCESS == retval) {
        (*thread_array)->threads = NULL;
        retval =  AMP_DEALLOC(allocator, *thread_array);
        if (AMP_SUCCESS == retval) {
            *thread_array = AMP_THREAD_ARRAY_UNINITIALIZED;
        } else {
            assert(0); /* Unable to deallocate - posibly bad dealloc_func */
            retval = AMP_ERROR;
        }
    }
    
    return retval;
}



int amp_thread_array_configure_contexts(amp_thread_array_t thread_array,
                                        size_t range_begin,
                                        size_t range_length,
                                        void* shared_context)
{
    size_t thread_count = 0;
    struct amp_raw_thread_s *threads = NULL;
    size_t range_end = 0;
    size_t i = 0;

    assert(NULL != thread_array);
    assert(range_begin < thread_array->thread_count);
    assert(range_length > 0);
    assert(range_length <= thread_array->thread_count);
    assert(range_begin <= thread_array->thread_count - range_length);
    assert(0 == thread_array->joinable_count);
    
    thread_count = thread_array->thread_count;
    
    if (range_begin >= thread_count
        || range_length <= 0
        || range_length > thread_count
        || range_begin > thread_count - range_length) {
        
        return AMP_ERROR;
    }
    if (0 != thread_array->joinable_count) {
        return AMP_BUSY;
    }
    
    threads = thread_array->threads;
    range_end = range_begin - 1 + range_length;
    
    for (i = range_begin; i <= range_end; ++i) {
        int const errc = amp_internal_thread_configure_context(&threads[i],
                                                               shared_context);
        if (AMP_SUCCESS != errc) {
            return errc;
        }
    }
    
    return AMP_SUCCESS;
}



int amp_thread_array_configure_functions(amp_thread_array_t thread_array,
                                         size_t range_begin,
                                         size_t range_length,
                                         amp_thread_func_t shared_function)
{
    size_t thread_count = 0;
    struct amp_raw_thread_s *threads = NULL;
    size_t range_end = 0;
    size_t i = 0;

    assert(NULL != thread_array);
    assert(range_begin < thread_array->thread_count);
    assert(range_length > 0);
    assert(range_length <= thread_array->thread_count);
    assert(range_begin <= thread_array->thread_count - range_length);
    assert(NULL != shared_function);
    assert(0 == thread_array->joinable_count);
    
    thread_count = thread_array->thread_count;
    
    if (range_begin >= thread_count
        || range_length <= 0
        || range_length > thread_count
        || range_begin > thread_count - range_length
        || NULL == shared_function) {
        
        return AMP_ERROR;
    }
    if (0 != thread_array->joinable_count) {
        return AMP_BUSY;
    }
    
    threads = thread_array->threads;
    range_end = range_begin - 1 + range_length;
    for (i = range_begin; i <= range_end; ++i) {
        int const errc = amp_internal_thread_configure_function(&threads[i],
                                                                shared_function);
        if (AMP_SUCCESS != errc) {
            return errc;
        }
    }
    
    return AMP_SUCCESS;
}



int amp_thread_array_configure(amp_thread_array_t thread_array,
                               size_t range_begin,
                               size_t range_length,
                               void* shared_context,
                               amp_thread_func_t shared_function)
{
    size_t thread_count = 0;
    struct amp_raw_thread_s* threads = NULL;
    size_t range_end = 0;
    size_t i = 0;

    assert(NULL != thread_array);
    assert(range_begin < thread_array->thread_count);
    assert(range_length > 0);
    assert(range_length <= thread_array->thread_count);
    assert(range_begin <= thread_array->thread_count - range_length);
    assert(NULL != shared_function);
    assert(0 == thread_array->joinable_count);
    
    thread_count = thread_array->thread_count;
    
    if (range_begin >= thread_count
        || range_length <= 0
        || range_length > thread_count
        || range_begin > thread_count - range_length
        || NULL == shared_function) {
        
        return AMP_ERROR;
    }
    if (0 != thread_array->joinable_count) {
        return AMP_BUSY;
    }
    
    threads = thread_array->threads;
    range_end = range_begin - 1 + range_length;
    for (i = range_begin; i <= range_end; ++i) {
        int errc0 = AMP_ERROR;
        int errc1 = AMP_ERROR;
        
        errc0 = amp_internal_thread_configure_context(&threads[i],
                                                      shared_context);
        errc1 = amp_internal_thread_configure_function(&threads[i],
                                                       shared_function);
        if (AMP_SUCCESS != errc0
            || AMP_SUCCESS != errc1) {
            
            return (AMP_SUCCESS != errc0 ? errc0 : errc1);
        }
    }
    
    return AMP_SUCCESS;
}


int amp_thread_array_launch_all(struct amp_thread_array_s *thread_array,
                                size_t* joinable_thread_count)
{
    struct amp_raw_thread_s *threads = NULL;
    size_t joinable_count = 0;
    size_t thread_count = 0;
    int retval = AMP_SUCCESS;
    
    assert(NULL != thread_array);
    
    threads = thread_array->threads;
    joinable_count = thread_array->joinable_count;
    thread_count = thread_array->thread_count;
    
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


int amp_thread_array_join_all(struct amp_thread_array_s* thread_array,
                              size_t* joinable_thread_count)
{
    struct amp_raw_thread_s *threads = NULL;
    size_t joinable_count = 0;
    size_t joined_count = 0;
    int retval = AMP_SUCCESS;
    
    assert(NULL != thread_array);
    
    threads = thread_array->threads;
    joinable_count = thread_array->joinable_count;
    joined_count = 0;
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
                                               size_t* joinable_thread_count)
{
    assert(NULL != thread_array);
    assert(NULL != joinable_thread_count);
    
    *joinable_thread_count = thread_array->joinable_count;
    
    return AMP_SUCCESS;
}



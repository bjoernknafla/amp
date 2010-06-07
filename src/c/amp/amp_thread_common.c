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
 * Cross platform functionality shared by all platform thread backends.
 * Look into the different amp_raw_thread_  backend source files for
 * the platform / backend specific implementations.
 *
 * TODO: @todo Decide if to rename the file to amp_raw_thread_shared.c
 */

#include "amp_raw_thread.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include "amp_stddef.h"
#include "amp_internal_thread.h"



int amp_internal_thread_init_for_configuration(amp_thread_t thread)
{
    assert(NULL != thread);
    
    if (NULL == thread) {
        return EINVAL;
    }
    
    thread->func = NULL;
    thread->func_context = NULL;
    thread->reserved0 = 0;
    thread->reserved1 = 0;
    thread->state = 0; /* Signal not initialized */
    
    int const retval = amp_internal_native_thread_set_invalid(&thread->native_thread_description);
    if (AMP_SUCCESS == retval) {
        /* Signal initialized */
            thread->state = (int)amp_internal_thread_prelaunch_state;
    }
    
    return retval;
}



int amp_internal_thread_configure(amp_thread_t thread,
                                  void* func_context,
                                  amp_thread_func_t func)
{
    assert(NULL != thread);
    assert(amp_internal_thread_joinable_state != thread->state);
    
    if (NULL == thread) {
        return EINVAL;
    }
    if (amp_internal_thread_joinable_state == thread->state) {
        return EBUSY;
    }
    
    
    thread->func = func;
    thread->func_context = func_context;
    
    return AMP_SUCCESS;
}



int amp_internal_thread_configure_context(amp_thread_t thread,
                                          void *context)
{
    assert(NULL != thread);
    assert(amp_internal_thread_joinable_state != thread->state);
    
    if (NULL == thread) {
        return EINVAL;
    }
    if (amp_internal_thread_joinable_state == thread->state) {
        return EBUSY;
    }
    
    thread->func_context = context;
    
    return AMP_SUCCESS;
}



int amp_internal_thread_configure_function(amp_thread_t thread,
                                           amp_thread_func_t func)
{
    assert(NULL != thread);
    assert(amp_internal_thread_joinable_state != thread->state);
    
    if (NULL == thread) {
        return EINVAL;
    }
    if (amp_internal_thread_joinable_state == thread->state) {
        return EBUSY;
    }
    
    thread->func = func;
    
    return AMP_SUCCESS;
}



int amp_internal_thread_context(amp_thread_t thread,
                                void **context)
{
    assert(NULL != thread);
    assert(NULL != context);
    
    if (NULL == thread
        || NULL == context) {
        
        return EINVAL;
    }
    
    *context = thread->func_context;
    
    return AMP_SUCCESS;
}



int amp_internal_thread_function(amp_thread_t thread,
                                 amp_thread_func_t *func)
{
    assert(NULL != thread);
    assert(NULL != func);
    
    if (NULL == thread
        || NULL == func) {
        
        return EINVAL;
    }
    
    *func = thread->func;
    
    return AMP_SUCCESS;
}



int amp_raw_thread_launch(amp_thread_t thread, 
                          void *func_context, 
                          amp_thread_func_t func)
{
    assert(NULL != func);
    
    if (NULL == func) {
        return EINVAL;
    }
    
    int retval = amp_internal_thread_init_for_configuration(thread);
    if (AMP_SUCCESS != retval) {
        return retval;
    }
    
    retval = amp_internal_thread_configure(thread,
                                           func_context,
                                           func);
    if (AMP_SUCCESS != retval)
    {
        return retval;
    }
    
    retval = amp_internal_thread_launch_configured(thread);
    
    return retval;
}



int amp_thread_create_and_launch(amp_thread_t *thread,
                                 void *allocator_context,
                                 amp_alloc_func_t alloc_func,
                                 amp_dealloc_func_t dealloc_func,
                                 void *func_context,
                                 amp_thread_func_t func)
{
    assert(NULL != thread);
    assert(NULL != alloc_func);
    assert(NULL != dealloc_func);
    assert(NULL != func);
    
    if (NULL == thread
        || NULL == alloc_func
        || NULL == dealloc_func
        || NULL == func) {
        
        return EINVAL;
    }
    
    amp_thread_t local_thread = (amp_thread_t)alloc_func(allocator_context,
                                                         sizeof(struct amp_raw_thread_s));
    
    if (NULL == local_thread) {
        return ENOMEM;
    }
    
    int retval = amp_raw_thread_launch(local_thread,
                                       func_context,
                                       func);
    
    if (AMP_SUCCESS == retval) {
        *thread = local_thread;
    } else {
        dealloc_func(allocator_context, local_thread);
    }
    
    return retval;
}



int amp_thread_join_and_destroy(amp_thread_t thread,
                                void *allocator_context,
                                amp_dealloc_func_t dealloc_func)
{
    assert(NULL != thread);
    assert(NULL != dealloc_func);
    
    if (NULL == thread
        || NULL == dealloc_func) {
        
        return EINVAL;
    }
    
    int retval = amp_raw_thread_join(thread);
    
    if (AMP_SUCCESS == retval) {
        dealloc_func(allocator_context, thread);
    }
    
    return retval;
}



/*
 *  amp_internal_raw_thread_common.c
 *  amp
 *
 *  Created by Björn Knafla on 07.01.10.
 *  Copyright 2010 Bjoern Knafla Parallelization + AI + Gamedev Consulting. All rights reserved.
 *
 */

/**
 * @file
 *
 * Cross platform functionality shared by all platform thread backends.
 * Look into the different amp_internal_raw_thread_  backend source files for
 * the platform / backend specific implementations.
 *
 * TODO: @todo Rename the file to amp_internal_raw_thread_shared.c
 */

#include "amp_internal_raw_thread.h"


#include <assert.h>
#include <errno.h>


#include "amp_stddef.h"



int amp_internal_raw_thread_configure(struct amp_raw_thread_s *thread,
                                      void* func_context,
                                      amp_raw_thread_func_t func)
{
    assert(NULL != thread);
    assert(amp_internal_raw_thread_joinable_state != thread->state);
    
    if (NULL == thread) {
        return EINVAL;
    }
    if (amp_internal_raw_thread_joinable_state == thread->state) {
        return EBUSY;
    }
    
    
    thread->thread_func = func;
    thread->thread_func_context = func_context;
    thread->state = (int)amp_internal_raw_thread_prelaunch_state;
    
    return AMP_SUCCESS;
}



int amp_internal_raw_thread_configure_context(struct amp_raw_thread_s *thread,
                                              void *context)
{
    assert(NULL != thread);
    
    if (NULL == thread) {
        return EINVAL;
    }
    if (amp_internal_raw_thread_joinable_state == thread->state) {
        return EBUSY;
    }
    
    thread->thread_func_context = context;
    thread->state = (int)amp_internal_raw_thread_prelaunch_state;
    
    return AMP_SUCCESS;
}



int amp_internal_raw_thread_configure_function(struct amp_raw_thread_s *thread,
                                               amp_raw_thread_func_t func)
{
    assert(NULL != thread);
    assert(amp_internal_raw_thread_joinable_state != thread->state);
    
    if (NULL == thread) {
        return EINVAL;
    }
    if (amp_internal_raw_thread_joinable_state == thread->state) {
        return EBUSY;
    }
    
    thread->thread_func = func;
    thread->state = (int)amp_internal_raw_thread_prelaunch_state;
    
    return AMP_SUCCESS;
}



int amp_internal_raw_thread_context(struct amp_raw_thread_s *thread,
                                    void **context)
{
    assert(NULL != thread);
    assert(NULL != context);
    
    if (NULL == thread
        || NULL == context) {
        
        return EINVAL;
    }
    
    *context = thread->thread_func_context;
    
    return AMP_SUCCESS;
}



int amp_internal_raw_thread_function(struct amp_raw_thread_s *thread,
                                     amp_raw_thread_func_t *func)
{
    assert(NULL != thread);
    assert(NULL != func);
    
    if (NULL == thread
        || NULL == func) {
        
        return EINVAL;
    }
    
    *func = thread->thread_func;
    
    return AMP_SUCCESS;
}




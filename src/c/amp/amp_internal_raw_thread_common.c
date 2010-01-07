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



int amp_internal_raw_thread_init(struct amp_raw_thread_s *thread,
                                 void* func_context,
                                 amp_raw_thread_func_t func)
{
    assert(NULL != thread);
    assert(NULL != func);
    
    if (NULL == thread || NULL == func) {
        return EINVAL;
    }
    
    thread->thread_func = func;
    thread->thread_func_context = func_context;
    thread->state = AMP_INTERNAL_RAW_THREAD_PRELAUNCH_STATE;
    
    return AMP_SUCCESS;
}

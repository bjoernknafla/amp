/*
 *  amp_raw_thread_common.c
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
 * Look into the different amp_raw_thread_  backend source files for
 * the platform / backend specific implementations.
 *
 * TODO: @todo Rename the file to amp_raw_thread_shared.c
 */

#include "amp_raw_thread.h"


#include <assert.h>


#include "amp_internal_raw_thread.h"
#include "amp_stddef.h"



int amp_raw_thread_launch(amp_raw_thread_t *thread, 
                          void *thread_func_context, 
                          amp_raw_thread_func_t thread_func)
{
    int retval = amp_internal_raw_thread_init(thread,
                                              thread_func_context,
                                              thread_func);
    assert(AMP_SUCCESS == retval);
    if (AMP_SUCCESS != retval)
    {
        return retval;
    }
    
    retval = amp_internal_raw_thread_launch_initialized(thread);
    
    return retval;
}

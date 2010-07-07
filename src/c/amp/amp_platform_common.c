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
 * Implementation of amp_platform functionality used by all platform 
 * specific implementations.
 */

#include "amp_platform.h"

#include <assert.h>
#include <stddef.h>

#include "amp_return_code.h"
#include "amp_raw_platform.h"


/**
 * Internal helper function returning the greater value of a and b.
 */
size_t amp_internal_max(size_t a, size_t b);
size_t amp_internal_max(size_t a, size_t b)
{
    return ((a >= b)? a : b);
}



/**
 * TODO: @todo Change the signature of amp_raw_platform_init or of the 
 *             platform create functions to make it explicit that an allocator
 *             is stored internally or mainly use a buffer per platform
 *             object.
 */
int amp_raw_platform_init(amp_platform_t descr,
                          amp_allocator_t allocator)
{
    assert(NULL != descr);
    assert(NULL != allocator);
    
    descr->allocator_context = allocator->allocator_context;
    descr->alloc_func = allocator->alloc_func;
    descr->dealloc_func = allocator->dealloc_func;
    
    return AMP_SUCCESS;
}


int amp_raw_platform_finalize(amp_platform_t descr,
                              amp_allocator_t allocator)
{
    (void)allocator;
    
    assert(NULL != descr);
    assert(NULL != allocator);
    
    descr->allocator_context = NULL;
    descr->alloc_func = NULL;
    descr->dealloc_func = NULL;
    
    return AMP_SUCCESS;
}



int amp_platform_create(amp_platform_t* descr,
                        amp_allocator_t allocator)
{
    amp_platform_t tmp_platform = AMP_PLATFORM_UNINITIALIZED;
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != descr);
    assert(NULL != allocator);
    
    *descr = AMP_PLATFORM_UNINITIALIZED;
    
    tmp_platform = (amp_platform_t)AMP_ALLOC(allocator,
                                              sizeof(*tmp_platform));
    if (NULL == tmp_platform) {
        return AMP_NOMEM;
    }
    
    retval = amp_raw_platform_init(tmp_platform,
                                   allocator);
    if (AMP_SUCCESS == retval) {
        *descr = tmp_platform;
    } else {
        int const rc = AMP_DEALLOC(allocator, tmp_platform);
        assert(AMP_SUCCESS == rc);
        (void)rc;
    }
    
    return retval;
}



int amp_platform_destroy(amp_platform_t* descr,
                         amp_allocator_t allocator)
{
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != descr);
    assert(NULL != *descr);
    assert(NULL != allocator);
    
    retval = amp_raw_platform_finalize(*descr,
                                       allocator);
    if (AMP_SUCCESS == retval) {
        retval = AMP_DEALLOC(allocator,
                              *descr);
        if (AMP_SUCCESS == retval) {
            *descr = AMP_PLATFORM_UNINITIALIZED;
        } else {
            retval = AMP_ERROR;
        }
    }
    
    return retval;
}



int amp_platform_get_concurrency_level(amp_platform_t descr,
                                       size_t* result)
{
    int return_value = AMP_UNSUPPORTED;

    int retcode_installed_hwthreads = AMP_UNSUPPORTED;
    int retcode_installed_cores = AMP_UNSUPPORTED;
    int retcode_active_hwthreads = AMP_UNSUPPORTED;
    int retcode_active_cores = AMP_UNSUPPORTED;
    
    size_t retval_installed_hwthreads = 0;
    size_t retval_installed_cores = 0;
    size_t retval_active_hwthreads = 0;
    size_t retval_active_cores = 0;
    
    assert(NULL != descr);
    
    retcode_installed_hwthreads = amp_platform_get_installed_hwthread_count(descr,
                                                                            &retval_installed_hwthreads);
    
    retcode_active_hwthreads = amp_platform_get_active_hwthread_count(descr,
                                                                      &retval_active_hwthreads);
    
    retcode_installed_cores = amp_platform_get_installed_core_count(descr,
                                                                    &retval_installed_cores);
    
    retcode_active_cores = amp_platform_get_active_core_count(descr,
                                                              &retval_active_cores);
    
    
    
    if (AMP_SUCCESS == retcode_installed_hwthreads 
        || AMP_SUCCESS == retcode_active_hwthreads
        || AMP_SUCCESS == retcode_installed_cores
        || AMP_SUCCESS == retcode_active_cores) {
        
        return_value = AMP_SUCCESS;
    }
    
    if (AMP_SUCCESS == return_value 
        && NULL != result) {
        
        *result = amp_internal_max(amp_internal_max(retval_installed_hwthreads, retval_installed_cores),
                                   amp_internal_max(retval_active_hwthreads, retval_active_cores));
    }

    return return_value;
}



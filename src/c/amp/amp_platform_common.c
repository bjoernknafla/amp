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
#include <errno.h>
#include <stddef.h>

#include "amp_stddef.h"
#include "amp_raw_platform.h"



int amp_raw_platform_init(amp_platform_t descr,
                          void* allocator_context,
                          amp_alloc_func_t alloc_func,
                          amp_dealloc_func_t dealloc_func)
{
    assert(NULL != descr);
    assert(NULL != alloc_func);
    assert(NULL != dealloc_func);
    
    if (NULL == descr
        || NULL == alloc_func
        || NULL == dealloc_func) {
        
        return EINVAL;
    }
    
    descr->allocator_context = allocator_context;
    descr->alloc_func = alloc_func;
    descr->dealloc_func = dealloc_func;
    
    return AMP_SUCCESS;
}


int amp_raw_platform_finalize(amp_platform_t descr)
{
    assert(NULL != descr);
    
    if (NULL == descr) {
        
        return EINVAL;
    }
    
    descr->allocator_context = NULL;
    descr->alloc_func = NULL;
    descr->dealloc_func = NULL;
    
    return AMP_SUCCESS;
}



int amp_platform_create(amp_platform_t* descr,
                        void* allocator_context,
                        amp_alloc_func_t alloc_func,
                        amp_dealloc_func_t dealloc_func)
{
    assert(NULL != descr);
    assert(NULL != alloc_func);
    assert(NULL != dealloc_func);
    
    *descr = AMP_PLATFORM_UNINITIALIZED;
    
    if (NULL == descr
        || NULL == alloc_func
        || NULL == dealloc_func) {
        
        return EINVAL;
    }
    
    amp_platform_t tmp_platform = (amp_platform_t)alloc_func(allocator_context,
                                                             sizeof(struct amp_raw_platform_s));
    if (NULL == tmp_platform) {
        return ENOMEM;
    }
    
    int const retval = amp_raw_platform_init(tmp_platform,
                                             allocator_context,
                                             alloc_func,
                                             dealloc_func);
    if (AMP_SUCCESS == retval) {
        *descr = tmp_platform;
    } else {
        int const rc = dealloc_func(allocator_context, tmp_platform);
        assert(AMP_SUCCESS == rc);
    }
    
    return retval;
}



int amp_platform_destroy(amp_platform_t descr,
                         void* allocator_context,
                         amp_alloc_func_t alloc_func,
                         amp_dealloc_func_t dealloc_func)
{
    assert(NULL != descr);
    assert(NULL != alloc_func);
    assert(NULL != dealloc_func);
    
    if (NULL == descr
        || NULL == alloc_func
        || NULL == dealloc_func) {
        
        return EINVAL;
    }
    
    int retval = amp_raw_platform_finalize(descr);
    if (AMP_SUCCESS == retval) {
        retval = dealloc_func(allocator_context,
                                    descr);
        assert(AMP_SUCCESS == retval);
    }
    
    return retval;
}



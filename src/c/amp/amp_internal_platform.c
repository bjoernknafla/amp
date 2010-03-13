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


#include "amp_internal_platform.h"


#include <assert.h>
#include <errno.h>


#include "amp_stddef.h"



int amp_platform_destroy(struct amp_platform_s* descr, 
                         void* allocator_context,
                         amp_dealloc_func_t dealloc_func)
{
    assert(NULL != descr);
    assert(NULL != dealloc_func);
    
    if (NULL == descr
        || NULL == dealloc_func) {
        
        return EINVAL;
    }
    
    dealloc_func(allocator_context, descr);
    
    return AMP_SUCCESS;
}



int amp_platform_get_core_count(struct amp_platform_s* descr, 
                                size_t* result)
{
    assert(NULL != descr);
    
    if (NULL == descr) {
        return EINVAL;
    }
    
    
    int retval = AMP_SUCCESS;
    
    if (0 == descr->core_count) {
        retval = ENOSYS;
    }
    
    if (NULL != result && AMP_SUCCESS == retval) {
        
        *result = descr->core_count;
    }
    
    return retval;
}



int amp_platform_get_active_core_count(struct amp_platform_s* descr, 
                                       size_t* result)
{
    assert(NULL != descr);
    
    if (NULL == descr) {
        return EINVAL;
    }
    
    
    int retval = AMP_SUCCESS;
    
    if (0 == descr->active_core_count) {
        retval = ENOSYS;
    }
    
    if (NULL != result && AMP_SUCCESS == retval) {
        
        *result = descr->active_core_count;
    }
    
    return retval;
}



int amp_platform_get_hwthread_count(struct amp_platform_s* descr, 
                                    size_t* result)
{
    assert(NULL != descr);
    
    if (NULL == descr) {
        return EINVAL;
    }
    
    
    int retval = AMP_SUCCESS;
    
    if (0 == descr->hwthread_count) {
        retval = ENOSYS;
    }
    
    if (NULL != result && AMP_SUCCESS == retval) {
        
        *result = descr->hwthread_count;
    }
    
    return retval;
}



int amp_platform_get_active_hwthread_count(struct amp_platform_s* descr, 
                                           size_t* result)
{
    assert(NULL != descr);
    
    if (NULL == descr) {
        return EINVAL;
    }
    
    
    int retval = AMP_SUCCESS;
    
    if (0 == descr->active_hwthread_count) {
        retval = ENOSYS;
    }
    
    if (NULL != result && AMP_SUCCESS == retval) {
        
        *result = descr->active_hwthread_count;
    }
    
    return retval;
}


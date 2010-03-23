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


#include "amp_raw_platform.h"


#include <errno.h>
#include <assert.h>


#include "amp_stddef.h"
#include "amp_internal_platform_win_system_info.h"
#include "amp_internal_platform_win_system_logical_processor_information.h"



#if _WIN32_WINNT < 0x0501
#   error Compile amp_raw_platform_windows_min_host_sdk_win2000.c for support of the target operating system.
#endif





#error fall back on other methods if GetLogicalProcessorInformation does not work.



int amp_raw_platform_get_installed_core_count(struct amp_raw_platform_s* descr, 
                                    size_t* result)
{
    assert(NULL != descr);
    
    if (NULL == descr) {
        return EINVAL;
    }
    
    struct amp_internal_platform_win_info_s info;
    info.installed_core_count = 0;
    info.active_core_count = 0;
    info.installed_hwthread_count = 0;
    info.active_hwthread_count = 0;
    
    
    int return_value = amp_internal_platform_win_system_logical_processor_information(&info,
                                                                                      descr->allocator_context,
                                                                                      descr->alloc_func,
                                                                                      descr->dealloc_func);
    
    /* Fall back on Win2000 compatible platform query function if 
     * the last call returned ENOSYS to indicate that the current platform
     * does not support the query method.
     */
    if (ENOSYS == return_value) {
        
        return_value = amp_internal_platform_win_system_info(&info);
    }
    
    if (AMP_SUCCESS == return_value) {
        
        if (0 == info.installed_core_count) {
            return_value = ENOSYS;
        } else {
            return_value = AMP_SUCCESS;
        }
        
        if (NULL != result && AMP_SUCCESS == return_value) {
            
            *result = info.installed_core_count;
        }
    }
    
    return return_value;
}



int amp_raw_platform_get_active_core_count(struct amp_raw_platform_s* descr, 
                                           size_t* result)
{
    (void)result;
    
    assert(NULL != descr);
    
    if (NULL == descr) {
        return EINVAL;
    }
    
    return ENOSYS;
}



int amp_raw_platform_get_installed_hwthread_count(struct amp_raw_platform_s* descr, 
                                        size_t* result)
{
    assert(NULL != descr);
    
    if (NULL == descr) {
        return EINVAL;
    }
    
    struct amp_internal_platform_win_info_s info;
    info.installed_core_count = 0;
    info.active_core_count = 0;
    info.installed_hwthread_count = 0;
    info.active_hwthread_count = 0;
    
    
    int return_value = amp_internal_platform_win_system_logical_processor_information(&info,
                                                                                      descr->allocator_context,
                                                                                      descr->alloc_func,
                                                                                      descr->dealloc_func);
    
    /* Fall back on Win2000 compatible platform query function if 
     * the last call returned ENOSYS to indicate that the current platform
     * does not support the query method.
     */
    if (ENOSYS == return_value) {
        
        return_value = amp_internal_platform_win_system_info(&info);
    }
    
    if (AMP_SUCCESS == return_value) {
        
        if (0 == info.installed_hwthread_count) {
            return_value = ENOSYS;
        } else {
            return_value = AMP_SUCCESS;
        }
        
        if (NULL != result && AMP_SUCCESS == return_value) {
            
            *result = info.installed_hwthread_count;
        }
    }
    
    return return_value;
}



int amp_raw_platform_get_active_hwthread_count(struct amp_raw_platform_s* descr, 
                                               size_t* result)
{
    (void)result;
    
    assert(NULL != descr);
    
    if (NULL == descr) {
        return EINVAL;
    }
    
    return ENOSYS;
}



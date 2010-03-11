/*
 * Copyright (c) 2009, Bjoern Knafla
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
 * Platform hardware detection via Windows GetSystemInfo for 
 * Windows 2000 Professional and Winows 2000 Server compatibility or, if
 * available uses GetNativeSystemInfo for compatibility
 * with Windows XP and Windows Server 2003 (_WIN32_WINNT >= 0x0501) even when
 * running in a WOW64 environment.
 * 
 * Unable to detect hardware threads alas simultaneous multithreading (SMT) and
 * unable to differentiate between all cores of the queried computer and the 
 * cores that are active/online during the detection.
 *
 * amp_platform_destroy and most of the query functions for amp_platform
 * are implemented in amp_internal_platform.c.
 *
 * See http://msdn.microsoft.com/en-us/library/ms724381(VS.85).aspx
 *
 * See http://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine
 *
 * See http://msdn.microsoft.com/en-us/library/ms724958(VS.85).aspx
 *
 * See http://msdn.microsoft.com/en-us/library/ms724953(VS.85).aspx
 *
 * See http://msdn.microsoft.com/en-us/library/aa383745(VS.85).aspx
 */

#include "amp_platform.h"


#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#include <assert.h>
#include <errno.h>


#include "amp_stddef.h"
#include "amp_internal_platform.h"



typedef void (WINAPI *GetNativeSystemInfoFunc)(LPSYSTEM_INFO);



static size_t amp_internal_platform_get_core_count(void);
static size_t amp_internal_platform_get_core_count(void)
{
    /* TODO: @todo Check if this returns full cores or hwthread (hyper-threads).
     */
    
    SYSTEM_INFO sysinfo = {}; /* ZeroMemory(&sysinfo, sizeof(SYSTEM_INFO)) */
    
    GetNativeSystemInfoFunc get_native_system_info_func = (GetNativeSystemInfoFunc) GetProcAddress(                                                                                 GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
    
    if (NULL != get_native_system_info_func) {
        get_native_system_info_func(&sysinfo);
    } else {
        GetSystemInfo(&sysinfo);
    }
    
    DWORD const physical_processor_count = sysinfo.dwNumberOfProcessors;
    
    return (size_t)physical_processor_count;
}



int amp_platform_create(struct amp_platform_s** descr,
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
    
    struct amp_platform_s* temp = (struct amp_platform_s*)alloc_func(allocator_context, 
                                                                     sizeof(struct amp_platform_s));
    
    if (NULL == temp) {
        return ENOMEM;
    }
    
    temp->core_count = amp_internal_platform_get_core_count();
    temp->active_core_count = 0;
    temp->hwthread_count = 0;
    temp->active_hwthread_count = 0;
    
    *descr = temp;
    
    return AMP_SUCCESS;
}



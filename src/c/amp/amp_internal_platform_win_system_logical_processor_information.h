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
 * Internal implementation of platform queries for Windows
 * based on SYSTEM_LOGICAL_PROCESSOR_INFORMATION data structure.
 *
 * Platform hardware detection via Windows GetLogicalProcessorInformation
 * for at least Windows Server 2003 or Windows Vista, 
 * Windows XP Professional x64 Edition, Windows XP with SP3. 
 *
 * Unable to detect active cores or hardware threads, can only detect installed
 * cores and hardware threads. On Windows earlier than Windows Vista cores and
 * hardware threads can not be differentiated.
 * 
 * @attention The implementation of GetLogicalProcessorInformation can only 
 *            handle a fixed number of cores or hardware threads - on platforms
 *            that support more cores or hardware threads it (and therefore the
 *            amp_platform query functions) only return informations about the 
 *            processor group of the processor the calling thread executes on at 
 *            the time of the call. Use amp_platform_windows_min_host_sdk_winserver2008_r2_or_windows7.c 
 *            for newer Windows platforms to support querying many core
 *            processor hardware.
 *
 * See http://msdn.microsoft.com/en-us/library/ms683194(VS.85).aspx
 *
 * See http://msdn.microsoft.com/en-us/library/ms725492(VS.85).aspx
 *
 * See http://msdn.microsoft.com/en-us/library/ms725491(VS.85).aspx
 *
 * See http://msdn.microsoft.com/en-us/library/aa383745(VS.85).aspx
 *
 * See http://msdn.microsoft.com/en-us/library/ms724833(VS.85).aspx
 *
 * See http://msdn.microsoft.com/en-us/library/ms725494(VS.85).aspx
 *
 *
 * TODO: @todo Find out if pre Windows Vista reports hardware threads or cores?
 */


#ifndef AMP_amp_internal_platform_win_system_logical_processor_information_H
#define AMP_amp_internal_platform_win_system_logical_processor_information_H


#include <amp/amp_internal_platform_win_info.h>
#include <amp/amp_memory.h>



#if defined(__cplusplus)
extern "C" {
#endif
    
    
    /**
     * Enters all queryable infos into info or marks values as
     * not queryable (e.g. sets them to zero).
     * 
     * The function allocates memory to hold a query buffer which is always 
     * deallocated before the function ends.
     * 
     * Returns AMP_SUCCESS if the query method is supported or
     * AMP_UNSUPPORTED if not supported, AMP_NOMEM if not memory could be 
     * allocated to query the platform infos, or AMP_ERROR if an unexpected  
     * error occured (indicates a programming error).
     *
     * info, alloc_func, and dealloc_func must not be invalid or NULL.
     *
     * On error no memory is leaked.
     * On error info is not changed or even touched.
     */
    int amp_internal_platform_win_system_logical_processor_information(struct amp_internal_platform_win_info_s* info,
                                                                       void* allocator_context,
                                                                       amp_alloc_func_t alloc_func,
                                                                       amp_dealloc_func_t dealloc_func);
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif


#endif /* AMP_amp_internal_platform_win_system_logical_processor_information_H */

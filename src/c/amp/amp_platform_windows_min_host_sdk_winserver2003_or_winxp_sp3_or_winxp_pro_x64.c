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
 * Platform hardware detection via Windows GetLogicalProcessorInformation
 * for at least Windows Server 2003 or Windows Vista, 
 * Windows XP Professional x64 Edition, Windows XP with SP3. 
 *
 * Unable to detect active cores or hardware threads, can only detect installed
 * cores and hardware threads. On Windows earlier than Windows Vista cores and
 * hardware threads can not be differentiated.
 * 
 * @ATTENTION The implementation of GetLogicalProcessorInformation can only 
 *            handle a fixed number of cores or hardware threads - on platforms
 *            that support more cores or hardware threads it (and therefore the
 *            amp_platform query functions) only return informations about the 
 *            processor group of the processor the calling thread executes on at 
 *            the time of the call. Use amp_platform_windows_min_host_sdk_winserver2008_r2_or_windows7.c 
 *            for newer Windows platforms to support querying many core
 *            processor hardware.
 *
 * amp_platform_destroy and most of the query functions for amp_platform
 * are implemented in amp_internal_platform.c.
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

#include "amp_platform.h"


#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#include <assert.h>
#include <errno.h>


#include "amp_stddef.h"
#include "amp_internal_platform.h"



#if _WIN32_WINNT < 0x0501
#   error Compile amp_platform_windows_min_host_sdk_win2000.c for support of the target operating system.
#endif



/**
 * Checks if current operating system version is greater or equal to the
 * version information passed via the parameters.
 *
 * Returns @c 0 if the comparison was error free or returns the error code if
 * an error occured. If an error occured the value result points to isn't 
 * changed.
 *
 * The comparison result (only valid of @c 0 is returned) is stored in the
 * result argument. result must not be NULL.
 */
static DWORD amp_internal_win_version_greater_or_equal(AMP_BOOL* result,
                                                       DWORD major_version,
                                                       DWORD minor_version,
                                                       DWORD major_service_pack,
                                                       DWORD minor_service_pack);
static DWORD amp_internal_win_version_greater_or_equal(AMP_BOOL* result,
                                                       DWORD major_version,
                                                       DWORD minor_version,
                                                       DWORD major_service_pack,
                                                       DWORD minor_service_pack)
{
    assert(NULL != result);
    
    DWORD error_code = 0;
    
    OSVERSIONINFOEX os_version_info_ex = {}; /* ZeroMemory(&os_version_info_ex, sizeof(OSVERSIONINFOEX));  */
    DWORDLONG os_query_condition_mask = 0;
    int const comparison_operation = VER_GREATER_EQUAL;
    
    os_version_info_ex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    os_version_info_ex.dwMajorVersion = major_version;
    os_version_info_ex.dwMinorVersion = minor_version;
    os_version_info_ex.wServicePackMajor = major_service_pack;
    os_version_info_ex.wServicePackMinor = minor_service_pack;
    
    
    VER_SET_CONDITION(os_query_condition_mask, 
                      VER_MAJORVERSION,
                      comparison_operation);
    VER_SET_CONDITION(os_query_condition_mask, 
                      VER_MINORVERSION, 
                      comparison_operation);
    VER_SET_CONDITION(os_query_condition_mask, 
                      VER_SERVICEPACKMAJOR,
                      comparison_operation);
    VER_SET_CONDITION(os_query_condition_mask, 
                      VER_SERVICEPACKMINOR,
                      comparison_operation);
    
    /* Non-equal comparison with FALSE because of the documentation.
     * See http://msdn.microsoft.com/en-us/library/ms725492(VS.85).aspx
     * It states that a non-zero value is returned if the comparison is
     * true - but does not state that @c TRUE is returned.
     */
    if (FALSE != VerifyVersionInfo(&os_version_info_ex, 
                                   VER_MAJORVERSION 
                                   | VER_MINORVERSION 
                                   | VER_SERVICEPACKMAJOR 
                                   | VER_SERVICEPACKMINOR,
                                   os_query_condition_mask)) {
        
        c*result = AMP_TRUE;
        error_code = 0;
        
    } else {        
        DWORD const last_error = GetLastError();
        
        if(ERROR_OLD_WIN_VERSION == last_error)
        {
            *result = AMP_FALSE;
            error_code = 0;
        } else {
            /* An error occured, do not touch result. */
            
            error_code = last_error;
        }
        
    }
    
    
    return error_code;
}



typedef enum AMP_INTERNAL_WIN_TYPE {
    AMP_INTERNAL_WORKSTATION_WIN_TYPE = VER_NT_WORKSTATION,
    AMP_INTERNAL_DOMAIN_CONTROLLER_WIN_TYPE = VER_NT_DOMAIN_CONTROLLER,
    AMP_INTERNAL_SERVER_WIN_TYPE = VER_NT_SERVER
} AMP_INTERNAL_WIN_TYPE;



/**
 * Checks if current operating system version and type (client or server os) is
 * greater or equal to the version information passed via the parameters.
 *
 * Returns @c 0 if the comparison was error free or returns the error code if
 * an error occured. If an error occured the value result points to isn't 
 * changed.
 *
 * The comparison result (only valid of @c 0 is returned) is stored in the
 * result argument. result must not be NULL.
 */
static DWORD amp_internal_win_version_greater_or_equal_and_type_equal(AMP_BOOL* result,
                                                                      DWORD major_version,
                                                                      DWORD minor_version,
                                                                      DWORD major_service_pack,
                                                                      DWORD minor_service_pack,
                                                                      AMP_INTERNAL_WIN_TYPE product_type);
static DWORD amp_internal_win_version_greater_or_equal_and_type_equal(AMP_BOOL* result,
                                                                      DWORD major_version,
                                                                      DWORD minor_version,
                                                                      DWORD major_service_pack,
                                                                      DWORD minor_service_pack,
                                                                      AMP_INTERNAL_WIN_TYPE product_type)
{
    assert(NULL != result);
    
    DWORD error_code = 0;
    
    OSVERSIONINFOEX os_version_info_ex = {}; /* ZeroMemory(&os_version_info_ex, sizeof(OSVERSIONINFOEX));  */
    DWORDLONG os_query_condition_mask = 0;
    int const comparison_operation = VER_GREATER_EQUAL;
    
    os_version_info_ex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    os_version_info_ex.dwMajorVersion = major_version;
    os_version_info_ex.dwMinorVersion = minor_version;
    os_version_info_ex.wServicePackMajor = major_service_pack;
    os_version_info_ex.wServicePackMinor = minor_service_pack;
    os_version_info_ex.wProductType = product_type;
    
    
    VER_SET_CONDITION(os_query_condition_mask, 
                      VER_MAJORVERSION,
                      comparison_operation);
    VER_SET_CONDITION(os_query_condition_mask, 
                      VER_MINORVERSION, 
                      comparison_operation);
    VER_SET_CONDITION(os_query_condition_mask, 
                      VER_SERVICEPACKMAJOR,
                      comparison_operation);
    VER_SET_CONDITION(os_query_condition_mask, 
                      VER_SERVICEPACKMINOR,
                      comparison_operation);
    VER_SET_CONDITION(os_query_condition_mask,
                      VER_PRODUCT_TYPE,
                      VER_EQUAL);
    
    /* Non-equal comparison with FALSE because of the documentation.
     * See http://msdn.microsoft.com/en-us/library/ms725492(VS.85).aspx
     * It states that a non-zero value is returned if the comparison is
     * true - but does not state that @c TRUE is returned.
     */
    if (FALSE != VerifyVersionInfo(&os_version_info_ex, 
                                   VER_MAJORVERSION 
                                   | VER_MINORVERSION 
                                   | VER_SERVICEPACKMAJOR 
                                   | VER_SERVICEPACKMINOR
                                   | VER_PRODUCT_TYPE,
                                   os_query_condition_mask)) {
        
        *result = AMP_TRUE;
        error_code = 0;
        
    } else {        
        DWORD const last_error = GetLastError();
        
        if(ERROR_OLD_WIN_VERSION == last_error)
        {
            *result = AMP_FALSE;
            error_code = 0;
        } else {
            /* An error occured, do not touch result. */
            
            error_code = last_error;
        }
        
    }
    
    
    return error_code;
}



/**
 * Returns the number of bits set to one in bitmask.
 */
static int amp_internal_count_set_bits_of_ulong_ptr(ULONG_PTR bitmask);
static int amp_internal_count_set_bits_of_ulong_ptr(ULONG_PTR bitmask)
{
    /* Code based on Henry S. Warren, Jr., Hacker's Delight, Addison-Wesley, 2003, 
     * p. 70.
     * 
     * However using a simpler algorithm that counts ever bit of an unsigned
     * integral type to enable a predictive number of loop runs to enable
     * loop unrolling and also simpler arithmetic than used in in the example at
     * http://msdn.microsoft.com/en-us/library/ms683194(VS.85).aspx
     *
     
    ULONG sum = bitmask;
    while (0 != bitmask) {
        bitmask = bitmask >> 1;
        sum = sum - bitmask;
    }
    
    return (int)sum;
    */
    
    DWORD const ulong_size = sizeof(ULONG)* CHAR_BIT;
    ULONG_PTR const rightmost_bit = (ULONG_PTR)1;
    int counter = 0;
    
    
    for (DWORD i = 1; i < ulong_size; ++i) {
        
        counter += (int)(rightmost_bit & bitmask);
        bitmask = bitmask >> 1;
    }
    
    return counter;
}



typedef BOOL (WINAPI *GetLogicalProcessorInformationFunc)(__out    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, __inout  PDWORD);



static AMP_BOOL amp_internal_platform_can_differentiate_core_and_hwthread_count(void);
static AMP_BOOL amp_internal_platform_can_differentiate_core_and_hwthread_count(void)
{
    /* Prior to Windows Vista no differentiation of simultaneous multithreading
     * (SMT).
     *
     * Function is based on documentation for GetLogicalProcessorInformation.
     *
     * TODO: @todo Does GetNativeSystemInfo return SMT infos? Then the 
     *             difference between it and GetLogicalProcessorInformation
     *             could be used to determine the number of cores and the
     *             number of hardware threads.
     */
    
    AMP_BOOL can_differentiate = AMP_FALSE;
    
    GetLogicalProcessorInformationFunc get_logical_processor_information_func = (GetLogicalProcessorInformationFunc) GetProcAddress(GetModuleHandle(TEXT("kernel32")),"GetLogicalProcessorInformation");
    
    if (NULL != get_logical_processor_information_func) {
        
        int const error_code = amp_internal_win_version_greater_or_equal(can_differentiate,
                                                                         6,
                                                                         0,
                                                                         0,
                                                                         0);
    }

    return can_differentiate;
}



static int amp_internal_platform_get_logical_processor_information(struct amp_platform_s* platform,
                                                    void* allocator_context,
                                                    amp_alloc_func_t alloc_func,
                                                    amp_dealloc_func_t dealloc_func);
static int amp_internal_platform_get_logical_processor_information(struct amp_platform_s* platform,
                                                                   void* allocator_context,
                                                                   amp_alloc_func_t alloc_func,
                                                                   amp_dealloc_func_t dealloc_func)
{    
    assert(NULL != platform);
    assert(NULL != alloc_func);
    assert(NULL != dealloc_func);
    
    
    GetLogicalProcessorInformationFunc get_logical_processor_information_func = (GetLogicalProcessorInformationFunc) GetProcAddress(GetModuleHandle(TEXT("kernel32")),"GetLogicalProcessorInformation");
    
    if (NULL == get_logical_processor_information_func) {
        
        /* Target computer doesn't run needed windows version. */
        
        return ENOSYS;
    }
    
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION sysinfo_buffer = NULL;
    DWORD sysinfo_buffer_size = 0;
    
    BOOL query_successful = get_logical_processor_information_func(NULL,
                                                                   &sysinfo_buffer_size);
    DWORD const get_size_error_code = GetLastError();
    
    if (FALSE != query_successful 
        || ERROR_INSUFFICIENT_BUFFER != get_size_error_code
        || 0 == sysinfo_buffer_size) {
        
        assert(FALSE == query_successful 
               && "Query with size 0 expected to fail and return needed buffer size.");
        
        assert(ERROR_INSUFFICIENT_BUFFER == get_size_error_code 
               && "Query with size 0 expected to fail and return needed buffer size.");
        
        assert(0 != sysinfo_buffer_size 
               && "Query with size 0 expected to fail and return needed buffer size.");
        
        
        return EAGAIN;
    }
    
    sysinfo_buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)alloc_func(allocator_context,
                                                                       sysinfo_buffer_size);
    if (NULL == sysinfo_buffer) {
        
        return ENOMEM;
    }
    
    query_successful = get_logical_processor_information_func(sysinfo_buffer, 
                                                              &sysinfo_buffer_size);
    if (FALSE == query_successful) {
        
        /* Query to easily access error code when debugging. */
        DWORD const last_error = GetLastError();
        
        assert(TRUE == query_successful);
        
        dealloc_func(allocator_context, sysinfo_buffer);
        
        return EAGAIN;
    }
        
    
    int core_count = 0;
    int hwthread_count = 0;
    
    
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION sysinfo = sysinfo_buffer;
    
    for (size_t i = 0; i < sysinfo_buffer_size; i = i + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)) {
        
        switch (sysinfo->Relationship) {
            case RelationProcessorCore:
                
                if (1 == sysinfo->ProcessorCore.Flag) {
                    
                    if (TRUE == amp_internal_platform_can_differentiate_core_and_hwthread_count()) {
                        
                        /* TODO: @todo Check if a flag of 1 counts one core and 
                         * its hardware threads or only hardware threads and
                         * the core is counter by an entry with a flag of 0.
                         */
                        ++core_count;
                        
                        hwthread_count += amp_internal_count_set_bits_of_ulong_ptr(sysinfo->ProcessorMask);
                        
                    } else {
                        
                        /* TODO: @todo Check if pre-Windows Vista cores or
                         * hardware threads are reported.
                         */
                        hwthread_count += amp_internal_count_set_bits_of_ulong_ptr(sysinfo->ProcessorMask);
                        
                    }
                
                } else {
                    
                    core_count += amp_internal_count_set_bits_of_ulong_ptr(sysinfo->ProcessorMask);
                    
                }
                break;
            default:
                /* Ignore unneeded info. */
                break;
        }
        
        ++sysinfo;
    }
    
    dealloc_func(allocator_context, sysinfo_buffer);
        
    platform->core_count = (size_t)core_count;
    platform->active_core_count = 0;
    platform->hwthread_count = (size_t)hwthread_count;
    platform->active_hwthread_count = 0;
    
    
    return AMP_SUCCESS;    
}


#error fall back on other methods if GetLogicalProcessorInformation does not work.



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
    
    temp->core_count = 0;
    temp->active_core_count = 0;
    temp->hwthread_count = 0;
    temp->active_hwthread_count = 0;
    
    
    int error_code = amp_internal_platform_get_logical_processor_information(temp,
                                                                            allocator_context,
                                                                             alloc_func,
                                                                             dealloc_func);
    
    if (AMP_SUCCESS != error_code) {
        
        
        /* TODO: @todo Fall back to other way of querying the platform 
         */
        
        
        dealloc_func(allocator_context, temp);
        
        
        /* TODO: @todo Check that no non-documented error codes are returned. 
         */
        return error_code;
        
    }
    
    
    
    
    *descr = temp;
    
    return AMP_SUCCESS;
}



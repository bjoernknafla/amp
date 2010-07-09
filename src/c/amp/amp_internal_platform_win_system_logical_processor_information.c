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




#include "amp_internal_platform_win_system_logical_processor_information.h"

#include <assert.h>
#include <limits.h>
#include <stddef.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "amp_stddef.h"
#include "amp_return_code.h"



#if _WIN32_WINNT < 0x0501
#   error Target Windows OS must be at least Windows Server 2003 or WindowsXP.
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
                                                       WORD major_service_pack,
                                                       WORD minor_service_pack);
static DWORD amp_internal_win_version_greater_or_equal(AMP_BOOL* result,
                                                       DWORD major_version,
                                                       DWORD minor_version,
                                                       WORD major_service_pack,
                                                       WORD minor_service_pack)
{
    DWORD error_code = 0;
    OSVERSIONINFOEX os_version_info_ex; 
    DWORDLONG os_query_condition_mask = 0;
    int const comparison_operation = VER_GREATER_EQUAL;
    
    ZeroMemory(&os_version_info_ex, sizeof(OSVERSIONINFOEX));

    assert(NULL != result);
    
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
                                                                      WORD major_service_pack,
                                                                      WORD minor_service_pack,
                                                                      AMP_INTERNAL_WIN_TYPE product_type);
static DWORD amp_internal_win_version_greater_or_equal_and_type_equal(AMP_BOOL* result,
                                                                      DWORD major_version,
                                                                      DWORD minor_version,
                                                                      WORD major_service_pack,
                                                                      WORD minor_service_pack,
                                                                      AMP_INTERNAL_WIN_TYPE product_type)
{
    DWORD error_code = 0;
    OSVERSIONINFOEX os_version_info_ex;
    DWORDLONG os_query_condition_mask = 0;
    int const comparison_operation = VER_GREATER_EQUAL;
    
    ZeroMemory(&os_version_info_ex, sizeof(OSVERSIONINFOEX));

    assert(NULL != result);
    
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
    /* Code based on Henry S. Warren, Jr., Hacker's Delight, Addison-Wesley, 
     * 2003, p. 70.
     * 
     * However using a simpler algorithm that counts ever bit of an unsigned
     * integral type to enable a predictive number of loop runs to enable
     * loop unrolling and also simpler arithmetic than used in in the example at
     * http://msdn.microsoft.com/en-us/library/ms683194(VS.85).aspx
     *
     * <code>
     * ULONG sum = bitmask;
     * while (0 != bitmask) {
     * bitmask = bitmask >> 1;
     * sum = sum - bitmask;
     * }
     *
     * return (int)sum;
     * </code>
     */
    
    DWORD const ulong_size = sizeof(ULONG)* CHAR_BIT;
    ULONG_PTR const rightmost_bit = (ULONG_PTR)1;
    int counter = 0;
    DWORD i = 1;
    
    for (i = 1; i < ulong_size; ++i) {
        
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
        
        int const error_code = amp_internal_win_version_greater_or_equal(&can_differentiate,
                                                                         6,
                                                                         0,
                                                                         0,
                                                                         0);
    }
    
    return can_differentiate;
}



int amp_internal_platform_win_system_logical_processor_information(struct amp_internal_platform_win_info_s* info,
                                                                   void* allocator_context,
                                                                   amp_alloc_func_t alloc_func,
                                                                   amp_dealloc_func_t dealloc_func)
{
    
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION sysinfo_buffer = NULL;
    DWORD sysinfo_buffer_size = 0;
    GetLogicalProcessorInformationFunc get_logical_processor_information_func;
    BOOL query_successful = FALSE;
    DWORD get_size_error_code = 0;
    int core_count = 0;
    int hwthread_count = 0;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION sysinfo;
    int error_code = AMP_UNSUPPORTED;
    size_t i = 0;

    assert(NULL != info);
    assert(NULL != alloc_func);
    assert(NULL != dealloc_func);
    
    
    
    get_logical_processor_information_func = (GetLogicalProcessorInformationFunc) GetProcAddress(GetModuleHandle(TEXT("kernel32")),"GetLogicalProcessorInformation");
    if (NULL == get_logical_processor_information_func) {
        
        /* Target computer doesn't run needed windows version. */
        
        return AMP_UNSUPPORTED;
    }
    
   
    
    query_successful = get_logical_processor_information_func(NULL,
                                                              &sysinfo_buffer_size);
    get_size_error_code = GetLastError();
    
    if (FALSE != query_successful 
        || ERROR_INSUFFICIENT_BUFFER != get_size_error_code
        || 0 == sysinfo_buffer_size) {
        
        return AMP_ERROR;
    }
    
    sysinfo_buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)alloc_func(allocator_context,
                                                                       sysinfo_buffer_size,
                                                                       __FILE__,
                                                                       __LINE__);
    if (NULL == sysinfo_buffer) {
        
        return AMP_NOMEM;
    }
    
    query_successful = get_logical_processor_information_func(sysinfo_buffer, 
                                                              &sysinfo_buffer_size);
    if (FALSE == query_successful) {
        DWORD last_error = 0;

        error_code = AMP_UNSUPPORTED;

        /* Query to easily access error code when debugging. */
        last_error = GetLastError();
        
        error_code = dealloc_func(allocator_context, sysinfo_buffer, __FILE__, __LINE__);
        assert(AMP_SUCCESS == error_code);
        
        return AMP_ERROR;
    }
    
    sysinfo = sysinfo_buffer;
    
    for (i = 0; i < sysinfo_buffer_size; i = i + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)) {
        
        switch (sysinfo->Relationship) {
            case RelationProcessorCore:
                
                if (1 == sysinfo->ProcessorCore.Flags) {
                    
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
    
    error_code = dealloc_func(allocator_context, sysinfo_buffer, __FILE__, __LINE__);
    assert(AMP_SUCCESS == error_code);
    
    
    info->installed_core_count = (size_t)core_count;
    info->active_core_count = 0;
    info->installed_hwthread_count = (size_t)hwthread_count;
    info->active_hwthread_count = 0;
    
    return AMP_SUCCESS;
}



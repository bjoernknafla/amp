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
 * Platform hardware detection via Windows GetLogicalProcessorInformation
 * for at least Windows Server 2003 or Windows Vista, 
 * Windows XP Professional x64 Edition, Windows XP with SP3.
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
 */

#include "amp_platform.h"


#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#if _WIN32_WINNT < 0x0501
#   error Compile amp_platform_win_getsysteminfo.c for support of the target operating system.
#elif _WIN32_WINNT >= 0x0601 /* TODO: @todo Check version number for Windows Server 2008 R2 */
#   error Compile amp_platform_win_getlogicalprocessorinformationex.c for better support of the target operating system.
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
int amp_internal_count_set_bits_of_ulong(ULONG bitmask);
int amp_internal_count_set_bits_of_ulong(ULONG bitmask)
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
    ULONG const rightmost_bit = (ULONG)1;
    int counter = 0;
    
    
    for (DWORD i = 1; i < ulong_size; ++i) {
        
        counter += (int)(rightmost_bit & bitmask);
        bitmask = bitmask >> 1;
    }
    
    return counter;
}



AMP_BOOL amp_platform_can_detect_core_count(void)
{
    return AMP_TRUE;
}



AMP_BOOL amp_platform_can_detect_hwthread_count(void)
{
    /* Prior to Windows Vista no differentiation of simultaneous multithreading
     * (SMT).
     * TODO: @todo How to detect the OS version at runtime?
     * TODO: @todo Does GetNativeSystemInfo return SMT infos? Then the 
     *             difference between it and GetLogicalProcessorInformation
     *             could be used to determine the number of cores and the
     *             number of hardware threads.
     */
    
    
    
    os version >= 6.0 && has GetLogicalProcessorInformation
   
    
    certain os versions can not detect difference of online and physical core count
    
#error Implement
}



//size_t amp_platform_get_core_count_online(void)
int amp_platform_get_core_count_online(size_t* result)
{
    
    
    
    
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION sysinfo_buffer[???];
    DWORD sysinfo_buffer_size = ???;
    
    BOOL const query_successful = GetLogicalProcessorInformation(&sysinfo_buffer,
                                                                 &sysinfo_buffer_size);
    
    if (FALSE == query_successful) {
        
        /* Retrieve last error to ease diagnstics when debugging. */
        DWORD const last_error = GetLastError();
        
        /* In debug mode assert to inform the dev about a problem. */
        assert(0);
        
        return 0;
    }
    
    
}


// Rename to get_physical_core_count?
size_t amp_platform_get_core_count_max(void)
{
}



size_t amp_platform_get_hwthread_count_online(void)
{
}



size_t amp_platform_get_hwthread_count_max(void)
{
}


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
 * based on SYSTEM_INFO data structure.
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


#ifndef AMP_amp_internal_platform_win_system_info_H
#define AMP_amp_internal_platform_win_system_info_H


#include <amp/amp_internal_platform_win_info.h>



#if defined(__cplusplus)
extern "C" {
#endif

    
    /**
     * Enters all queryable infos into info or marks values as
     * not queryable (e.g. sets them to zero).
     * 
     * Returns AMP_SUCCESS if the query method is supported or
     * AMP_UNSUPPORTED otherwise.
     *
     * info must not be invalid or NULL.
     *
     * On error info is not changed or even touched.
     */
    int amp_internal_platform_win_system_info(struct amp_internal_platform_win_info_s* info);
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif
    

#endif /* AMP_amp_internal_platform_win_system_info_H */

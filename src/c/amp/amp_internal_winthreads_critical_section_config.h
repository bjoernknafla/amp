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
 * Standard settings for Windows CRITICAL_SECTION 
 * InitializeCriticalSectionAndSpinCount function.
 */

#ifndef AMP_amp_internal_winthreads_critical_section_config_H
#define AMP_amp_internal_winthreads_critical_section_config_H


#define AMP_RAW_MUTEX_WINTHREADS_CRITICAL_SECTION_ZERO_SPIN_COUNT 0
#define AMP_RAW_MUTEX_WINTHREADS_CRITICAL_SECTION_DEFAULT_SPIN_COUNT 1500


/*
 * Flag or-ed into the spin count when calling 
 * @c InitializeCriticalSectionAndSpinCount . On Win2000 the flag triggers
 * an immediate initialization instead of the Win2000-typical lazy 
 * initialization to prevent later otherwise possible exceptions when entering
 * or leaving the CRITICAL_SECTION. Later Windows version won't throw 
 * exceptions and ignore the high bit set by this flag.
 * 
 * See http://msdn.microsoft.com/en-us/library/ms683476(VS.85).aspx for details.
 */
#define AMP_RAW_MUTEX_WINTHREADS_CRITICAL_SECTION_CREATE_IMMEDIATELY_ON_WIN2000 0x080000000


#endif /* AMP_amp_internal_winthreads_critical_section_config_H */

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
 * Backend specific definition of amp condition variable to allow placing a 
 * condition variable on the stack. Platform specific headers will be 
 * included.
 *
 * @attention Don't copy a variable of type amp_raw_condition_variable_s - 
 *            copying a pointer to this type is ok though.
 */

#ifndef AMP_amp_raw_condition_variable_H
#define AMP_amp_raw_condition_variable_H

#include <amp/amp_condition_variable.h>



#if defined(AMP_USE_PTHREADS)
#   include <pthread.h>
#elif defined(AMP_USE_WINVISTA_CONDITION_VARIABLES)
#   define WIN32_LEAN_AND_MEAN /* Only include streamlined windows header. */
#   if !define(_WIN32_WINNT) || (_WIN32_WINNT < 0x0601) /* To support CONDITION_VARIABLE */
#       error Windows version not supported.
#   endif
#   include <windows.h>
#elif defined(AMP_USE_WINTHREADS)
#   define WIN32_LEAN_AND_MEAN /* Only include streamlined windows header. */
#   if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0403) /* To support CRITICAL_SECTION */
#       error Windows version not supported.
#   endif
#   include <windows.h>
#else
#   error Unsupported platform.
#endif



#if defined(__cplusplus)
extern "C" {
#endif

    
    /**
     * Don't copy and don't move, pointer can be copied and moved but ownership
     * management is up to the user.
     */
    struct amp_raw_condition_variable_s
    {
#if defined(AMP_USE_PTHREADS)
        pthread_cond_t cond;
#elif defined(AMP_USE_WINVISTA_CONDITION_VARIABLES)
        CONDITION_VARIABLE cond;
#elif defined(AMP_USE_WINTHREADS)
        CRITICAL_SECTION access_waiting_threads_count_critsec;
        CRITICAL_SECTION wake_waiting_threads_critsec;
        HANDLE waking_waiting_threads_count_control_sem;
        HANDLE finished_waking_waiting_threads_event;
        LONG waiting_thread_count;
        /* TODO: @todo Should this be a 32/64bit var instead of a BOOL to enable
         *             alignment and atomicity? 
         */
        BOOL broadcast_in_progress;
#else
#   error Unsupported platform.
#endif
    };
    
    
    
    /**
     * Like amp_condition_variable_create but does not allocate memory for the 
     * amp conditon variable other than indirectly via the platform API to 
     * create a platform condition variable.
     */
    int amp_raw_condition_variable_init(amp_condition_variable_t cond);
    
    /**
     * Like amp_condition_variable_destroy but does not free memory for the amp 
     * condition variable other than indirectly via the platform API to destroy 
     * a platform condition variable.
     */
    int amp_raw_condition_variable_finalize(amp_condition_variable_t cond);
    
    
    

#if defined(__cplusplus)
} /*  extern "C" */
#endif    
    
    
#endif /* AMP_amp_raw_condition_variable_H */

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
 * Backend specific definition of amp mutex to allow placing a mutex on the 
 * stack though platform specific headers will be included.
 *
 * @attention Don't copy a variable of type amp_raw_mutex_s - copying a pointer
 *            to this type is ok though.
 *
 * TODO: @todo Add Apple OS X 10.6 libdispatch backends for amp_raw_ mutex.
 */


#ifndef AMP_amp_raw_mutex_H
#define AMP_amp_raw_mutex_H

#include <amp/amp_mutex.h>



#if defined(AMP_USE_PTHREADS)
#   include <pthread.h>
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
     * Simple non-recursive mutex to synchronization inside the owning process.
     * No inter-process syncing supported.
     *
     * Treat definition and size as opaque as these can change without a 
     * warning in future versions of amp.
     *
     * @attention Don't copy or move an amp_raw_mutex instance or behavior is
     *            undefined - use pointers to an amp_raw_mutex instead.
     */
    struct amp_raw_mutex_s {
#if defined(AMP_USE_PTHREADS)
        /* Don't copy or move - therefore don't copy or move amp_mutex_s. */
        pthread_mutex_t mutex;
#elif defined(AMP_USE_WINTHREADS)
        /* Don't copy or move - therefore don't copy or move amp_mutex_s. */
        CRITICAL_SECTION critical_section;
        /* Helper to prevent recursive locking on Windows. This is always
         * included instead of only in debug mode because the interface and the
         * way the source is compiled can differ which might lead to hard to 
         * track down errors.
         */
        BOOL is_locked;
#else
#   error Unsupported platform.
#endif
    };
    

    
    /**
     * Like amp_mutex_create but does not allocate memory for the amp mutex
     * other than indirectly via the platform API to create a platform mutex.
     */
    int amp_raw_mutex_init(amp_mutex_t mutex);
    
    /**
     * Like amp_mutex_destroy but does not free memory for the amp mutex
     * other than indirectly via the platform API to destroy a platform mutex.
     */
    int amp_raw_mutex_finalize(amp_mutex_t mutex);
    
    

#if defined(__cplusplus)
} /* extern "C" */
#endif

        
#endif /* AMP_amp_raw_mutex_H */

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
 * Shallow wrapper around the platforms threads. Threads launched must always
 * be joined to prevent resource leaks. Currently threads can't be detached.
 *
 * TODO: @todo Manage threads when using cocoa (create at least one NSTask and create an autoreleasepool inside the threads.
 * TODO: @todo Rename this file and its functions to raw threads and create a amp_thread ADT or a amp thread cluster that collects threads and controls the functions running on the thread. Then add semaphores, mutexes, locks, atomic barriers and ops, and perhaps other stuff as needed.
 * TODO: @todo Add docu to say how many threads can run at max and if joined thread slots are recycled or are blocked for the runtime of the app. Decide if the id is really necessary.
 *
 * TODO: @todo Decide, implement and document if amp raw threads surpress 
 *             signals from reaching them.
 */


#ifndef AMP_amp_raw_thread_H
#define AMP_amp_raw_thread_H


#include <amp/amp_thread.h>



#if defined(AMP_USE_PTHREADS)
#   include <pthread.h>
#   include <stdint.h>
#elif defined(AMP_USE_WINTHREADS)
#   define WIN32_LEAN_AND_MEAN
#   if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0400) /* To support SwitchToThread function */
#       error Windows version not supported.
#   endif
#   include <windows.h>
#else
#   error Unsupported platform.
#endif



#if defined(__cplusplus)
extern "C" {
#endif

    
    
#define AMP_INVALID_THREAD_ID ((amp_thread_id_t)0)
    
    
    /**
     * Treat this type as opaque as its internals will change from version to
     * version!
     *
     * Stores the context and the thread function and hands the thread struct
     * to the thread start function (trampoline function) so it has access
     * to the function to run and the context to pass to it.
     */
    struct amp_native_thread_s {
#if defined(AMP_USE_PTHREADS)
        pthread_t thread;
#elif defined(AMP_USE_WINTHREADS)
        HANDLE thread_handle;
        DWORD thread_id;
#else
#   error Unsupported platform.        
#endif
    };
    
    /**
     * Treat this type as opaque as its internals will change from version to
     * version!
     */
    struct amp_raw_thread_s {
        amp_thread_func_t func;
        void *func_context;
        
        /* Reserved to store the allocator context and dealloc function to
         * store for automatic memory freeing when thread detaching is 
         * implemented.
         */
        uintptr_t reserved0;
        uintptr_t reserved1;
        
        struct amp_native_thread_s native_thread_description;
        
        /**
         * TODO: @todo The moment the amp atomic operations are ready make it
         *             an atomically changed flag that is queryable. Currently
         *             it can only be used by the thread that creates and
         *             launches (both by the same thread) the amp thread in a 
         *             reliable way.
         */
        int state;
    };
    
    
    
    /**
     * Like amp_thread_launch but does not allocate memory for the thread
     * other than indirectly via the platform API to launch a thread.
     */
    int amp_raw_thread_launch(amp_thread_t thread, 
                              void *thread_func_context, 
                              amp_thread_func_t thread_func);
    
    /**
     * Like amp_thread_join_and_destroy but does not free memory other than
     * indirectly via the platform API to join with the thread.
     */
    int amp_raw_thread_join(amp_thread_t thread);
    
    
    
#if defined(__cplusplus)
}    
#endif


#endif /* #ifndef AMP_amp_raw_thread_H */



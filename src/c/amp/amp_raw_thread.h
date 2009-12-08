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

/*    
#if defined(AMP_USE_PTHREADS)
#   define AMP_RAW_THREAD_COUNT_MAX PTHREAD_THREADS_MAX
#elif defined(AMP_USE_WINTHREADS)
#   error Implement
#else
#   error Unsupported platform.
#endif
  */  
    
    
    /**
     * Type of the user function to run on the thread.
     * @param context user data passed into the function.
     */
    typedef void (*amp_raw_thread_func_t)(void *context);
    
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
    typedef struct amp_native_thread_s amp_native_thread_t;
    
    /**
     * Treat this type as opaque as its internals will change from version to
     * version!
     */
    struct amp_raw_thread_s {
        amp_raw_thread_func_t thread_func;
        void *thread_func_context;
        
        struct amp_native_thread_s native_thread_description;
        
        int state;
    };
    typedef struct amp_raw_thread_s amp_raw_thread_t;
    
    
#if defined(AMP_USE_PTHREADS)  
    typedef uintptr_t amp_raw_thread_id_t;
#elif defined(AMP_USE_WINTHREADS)
    typedef DWORD amp_raw_thread_id_t;
#else
#   error Unsupported platform.
#endif
    
    
    /**
     * Calls platforms thread creation function that might call malloc 
     * internally and launches the thread.
     *
     * thread must be kept alive as long as the thread exectures - until
     * amp_raw_thread_join returned.
     *
     * Mental thread model: a thread is like a spaceship. After launch
     * it needs to explicitly land - otherwise it just crashes with undefined
     * behavior for the whole surrounding of the crash-site.
     *
     * Might call malloc internally to create platform internal thread 
     * representation.
     *
     * If fed with already launched native thread behavior is undefined.
     *
     * @return AMP_SUCCESS on successful thread launch.
     *         EAGAIN if the system is lacking resources for thread creation.
     *         Other error codes might be returned to signal errors while
     *         launching, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         EINVAL The thread or the thread function is invalid.
     *
     * @attention Passing NULL for @a thread or @a thread_func results in 
     *            undefined behavior.
     *
     * @attention Don't pass an already existing thread in or you won't be able
     *            to join it which results in a resource leak.
     *
     * TODO: @todo Add restrict to the pointers and document it.
     */
    int amp_raw_thread_launch(amp_raw_thread_t *thread, 
                               void *thread_func_context, 
                               amp_raw_thread_func_t thread_func);
    
    /**
     * Waits until the thread stops and frees its OS resources. 
     *
     * If thread hasn't been launched behavior is undefined.
     *
     * Thread memory can be freed after successful join.
     *
     * @return AMP_SUCCESS after succesfully joining with the thread.
     *         Other error codes might be returned to signal errors while
     *         joining, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         EDEADLK If a deadlock condition was detected or the calling
     *         thread tries to join with itself.
     *         EINVAL argument doesn't refer to a joinable thread.
     *         ESRCH if no  system thread is associated with the argument.
     *
     * @attention Only pass a thread as an argument if its launching was 
     *            successful, otherwise behavior is undefined.
     *
     * @attention Don't try to join a thread with itself, otherwise behavior is
     *            undefined.
     */
    int amp_raw_thread_join(amp_raw_thread_t *thread);
    
    
    
    /**
     * Returns the thread id of the thread calling the function.
     * The id can be compared to other ids.
     * When a thread is joined its thread id might be reused by a newly started
     * thread, therefore beware of joining with this new thread if you believe
     * that its the old thread.
     */
    amp_raw_thread_id_t amp_raw_thread_id(void);
    
    
    /**
     * Returns the thread id associated with the amp_raw_thread_t argument.
     * 
     * @attention Don't pass an non-launched or invalid thread as an argument.
     */
    amp_raw_thread_id_t amp_raw_thread_get_id(amp_raw_thread_t *thread);
    
    /**
     * Hints the operating system that it might be benificial to context switch
     * to another thread.
     *
     * Suggestion from David R. Butenhof's book 
     * "Programming with POSIX threads", p. 316 : call before locking a mutex to 
     * minimize the chance of a timeslice while the mutex is locked.
     *
     * @return AMP_SUCCESS on success, otherwise
     *         ENOSYS is returned if not supported.
     */
    int amp_raw_thread_yield(void);
    
    
    
#if defined(__cplusplus)
}    
#endif


#endif /* #ifndef AMP_amp_raw_thread_H */



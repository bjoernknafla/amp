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
 * TODO: @todo Manage threads when using cocoa (create at least one NSTask and 
 *             create an autoreleasepool inside the threads.
 * TODO: @todo Add docu to say how many threads can run at max and if joined 
 *             thread slots are recycled or are blocked for the runtime of the 
 *             app. Decide if the id is really necessary.
 * TODO: @todo Decide, implement and document if amp threads surpress 
 *             signals from reaching them.
 */

#ifndef AMP_amp_thread_H
#define AMP_amp_thread_H


#include <stddef.h>

#include <amp/amp_stdint.h>
#include <amp/amp_memory.h>



#if defined(__cplusplus)
extern "C" {
#endif

    
#define AMP_THREAD_UNINITIALIZED NULL
    
    /**
     * Type of the user function to run on the thread.
     * @param context user data passed into the function.
     */
    typedef void (*amp_thread_func_t)(void *context);
    
    
    /**
     * Identifier of a thread.
     */
    typedef uintptr_t amp_thread_id_t;
    
    /**
     * Opaque thread type.
     */
    typedef struct amp_raw_thread_s *amp_thread_t;
    
    
    /**
     * Creates and launches a thread.
     *
     * thread must be kept alive as long as the thread executes - which means 
     * until a call to amp_raw_thread_join on the thread returned.
     *
     * Mental thread model: a thread is like a spaceship. After launch
     * it needs to explicitly land (join) - otherwise it just crashes with 
     * undefined behavior for the whole surrounding of the crash-site.
     *
     * Might call the systems malloc internally to create platform internal  
     * thread representation.
     *
     * If fed with already launched native thread behavior is undefined.
     *
     * The allocator parameters are placed before the thread context and
     * function argument to enable use of the same argument order if 
     * lambda support is added to C (see Apple's Grand Central Dispatch and
     * Apple's C extension called blocks).
     * 
     * @return AMP_SUCCESS on successful thread launch.
     *         EAGAIN if the system is lacking resources for thread creation.
     *         Other error codes might be returned to signal errors while
     *         launching, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         EINVAL The thread or the thread function is invalid.
     *
     * @attetion A platform might restrict the number of threads which can be
     *           created system wide or per process.
     *
     * @attention Passing NULL for @a thread or @a func results in 
     *            undefined behavior.
     *
     * @attention Don't pass an already existing thread in or you won't be able
     *            to join with it which results in a resource leak and undefined
     *            behavior.
     *
     * TODO: @todo Add restrict to the pointers and document it.
     */
    int amp_thread_create_and_launch(amp_thread_t *thread,
                                     void *allocator_context,
                                     amp_alloc_func_t alloc_func,
                                     amp_dealloc_func_t dealloc_func,
                                     void *func_context,
                                     amp_thread_func_t func);
    
    /**
     * Waits until the thread stops and frees its resources. 
     *
     * If thread hasn't been launched behavior is undefined.
     *
     * If joining with a thread was not successful the memory of thread will not
     * be freed.
     *
     * allocator_context and dealloc_func must be capable of freeing the memory
     * allocated via the create function otherwise behavior is undefined and
     * resources might be leaked.
     *
     * @return AMP_SUCCESS after succesfully joining with the thread.
     *         Other error codes might be returned to signal errors while
     *         joining, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         EDEADLK might be reported if a deadlock condition was detected or
     *         the calling thread tries to join with itself.
     *         EINVAL might be reported if the argument doesn't refer to a 
     *         joinable thread.
     *         ESRCH might be returned if no  system thread is associated with 
     *         the argument.
     *
     * @attention Only pass a thread as an argument if its launching was 
     *            successful, otherwise behavior is undefined.
     *
     * @attention Don't try to join a thread with itself, otherwise behavior is
     *            undefined.
     */
    int amp_thread_join_and_destroy(amp_thread_t thread,
                                    void *allocator_context,
                                    amp_dealloc_func_t dealloc_func);

    /**
     * Returns the thread id of the thread calling the function.
     * The id can be compared to other ids.
     * When a thread is joined its thread id might be reused by a newly started
     * thread, therefore beware of joining with this new thread if you believe
     * that its the old thread.
     */
    amp_thread_id_t amp_thread_current_id(void);
    
    /**
     * Returns the thread id associated with the amp_thread_t argument.
     * 
     * @attention Don't pass an non-launched or invalid thread as an argument.
     */
    int amp_thread_id(amp_thread_t thread,
                      amp_thread_id_t *id);
    
    /**
     * Hints the operating system that it might be benificial to context switch
     * to another thread.
     *
     * Suggestion from David R. Butenhof's book 
     * "Programming with POSIX threads", p. 316 : call before locking a mutex to 
     * minimize the chance of a context switch while the mutex is locked.
     *
     * @return AMP_SUCCESS on success, otherwise
     *         ENOSYS is returned if not supported.
     */
    int amp_thread_yield(void);
    
    

#if defined(__cplusplus)
} /* extern "C" */
#endif


#endif /* AMP_amp_thread_H */

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
 * Internal helper functions that should never be used directly by amp users
 * because everything contained in this file can change without any notice.
 *
 * TODO: @todo When introducing export macros and statements make sure to hide
 *             internals.
 *
 * TODO: @todo When adding the ability to detach a thread add a check to the
 *             native thread adapter function to check for detachment and to
 *             store and free the memory resources used by the thread. The 
 *             detach function should therefore take an allocator context and
 *             dealloc function argument.
 *
 * TODO: @todo amp_internal_thread_id querying is a terrible hack on Pthreads
 *             right now. Fix it the moment atomics are supported and move
 *             the whole thread id querying to the public thread header.
 */


#ifndef AMP_amp_internal_thread_H
#define AMP_amp_internal_thread_H

#include <amp/amp_thread.h>
#include <amp/amp_raw_thread.h>



#if defined(__cplusplus)
extern "C" {
#endif

    
    /**
     * Identifier of a thread.
     */
    typedef uintptr_t amp_internal_thread_id_t;
    
#define AMP_INTERNAL_INVALID_THREAD_ID ((amp_internal_thread_id_t)0)
    
    
    
    
    /**
     * An initialized amp thread can be in the prelaunch state to configure it,
     * in the joinable state after it has been launched but before it has been
     * joined, or in the joined state.
     *
     * TODO: @todo Add possibility to detach an amp thread. Look at all 
     *             occurences of joinable and joined states to handle the
     *             detached state, too.
     */
    enum amp_internal_thread_state {
        amp_internal_thread_prelaunch_state = 1,
        amp_internal_thread_joinable_state,
        /* amp_internal_thread_detached_state, Not implemented yet */
        amp_internal_thread_joined_state,
    };
    typedef enum amp_internal_thread_state amp_internal_thread_state_t;
    
    
    /**
     * Sets the fields of native_thread so it represent an invalid thread.
     *
     * native_thread must not be NULL.
     *
     * @return AMP_SUCCESS if native_thread is not NULL, otherwise retuns 
     *         AMP_ERROR.
     */
    int amp_internal_native_thread_set_invalid(struct amp_native_thread_s *native_thread);
    
    
    /**
     * Only call on non-launched threads.
     * 
     * Initializes all internal fields to zero and sets the thread state to
     * amp_internal_thread_prelaunch_state.
     */
    int amp_internal_thread_init_for_configuration(amp_thread_t thread);
    
    
    /**
     * Set the user context and user function of an amp thread.
     *
     * A race condition can occur if calling from different threads for the same
     * thread object.
     *
     * @attention Don't call for launched and not yet joined threads.
     */
    int amp_internal_thread_configure(amp_thread_t thread,
                                      void* func_context,
                                      amp_thread_func_t func);
    
    /**
     * Sets the threads context.
     *
     * A race condition can occur if calling from different threads for the same
     * thread object.
     *
     * @attention Don't call for launched and not yet joined threads.
     */
    int amp_internal_thread_configure_context(amp_thread_t thread,
                                              void *context);
    
    
    /**
     * Set the thread function.
     *
     * A race condition can occur if calling from different threads for the same
     * thread object.
     *
     * @attention Don't call for launched and not yet joined threads.
     */
    int amp_internal_thread_configure_function(amp_thread_t thread,
                                               amp_thread_func_t func);
    
    /**
     * Returns the thread context in *context.
     *
     * The returned value can be invalid if the function is called while other
     * functions manipulate the thread and its fields.
     *
     * Based on the memory model the value might not be consistent between
     * threads.
     */
    int amp_internal_thread_context(amp_thread_t thread,
                                    void **context);
    
    /**
     * Returns the thread function in *func.
     *
     * The returned value can be invalid if the function is called while other
     * functions manipulate the thread and its fields.
     *
     * Based on the memory model the value might not be consistent between
     * threads.
     */
    int amp_internal_thread_function(amp_thread_t thread,
                                     amp_thread_func_t *func);
    
    
    /**
     * @attention Don't call for launched and not yet joined threads.
     */
    int amp_internal_thread_launch_configured(amp_thread_t thread);
    
    
    
    /**
     * Returns the thread id of the thread calling the function.
     * The id can be compared to other ids.
     * When a thread is joined its thread id might be reused by a newly started
     * thread, therefore beware of joining with this new thread if you believe
     * that its the old thread.
     *
     * @attention Work for the main thread but behavior is undefined if called
     *            from non-amp-created threads.
     */
    amp_internal_thread_id_t amp_internal_thread_current_id(void);
    
    /**
     * Returns the thread id associated with the amp_thread_t argument.
     * 
     * @attention Don't pass an non-launched or invalid thread as an argument.
     */
    int amp_internal_thread_id(amp_thread_t thread,
                      amp_internal_thread_id_t* id);
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif
    

#endif /* AMP_amp_internal_thread_H */

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
 * because everything contained in this file can change without any notice!
 *
 * TODO: @todo When introducing export macros and statements make sure to hide
 *             internals.
 */


#ifndef AMP_amp_internal_raw_thread_H
#define AMP_amp_internal_raw_thread_H


#include <amp/amp_raw_thread.h>



#if defined(__cplusplus)
extern "C" {
#endif

    
    /**
     * An initialized amp thread can be in the prelaunch state to configure it,
     * in the joinable state after it has been launched but before it has been
     * joined, or in the joined state.
     *
     * TODO: @todo Add possibility to detach an amp thread. Look at all 
     *             occurences of joinable and joined states to handle the
     *             detached state, too.
     */
    enum amp_internal_raw_thread_state {
        amp_internal_raw_thread_prelaunch_state = 1,
        amp_internal_raw_thread_joinable_state,
        /* amp_internal_raw_thread_detached_state, Not implemented yet */
        amp_internal_raw_thread_joined_state,
    };
    typedef enum amp_internal_raw_thread_state amp_internal_raw_thread_state_t;
    
    
    
    /**
     * Set the user context and user function of an amp thread.
     *
     * A race condition can occur if calling from different threads for the same
     * thread object.
     *
     * @attention Don't call for launched and not yet joined threads.
     */
    int amp_internal_raw_thread_configure(struct amp_raw_thread_s *thread,
                                          void* func_context,
                                          amp_raw_thread_func_t func);
    
    /**
     * Sets the threads context.
     *
     * A race condition can occur if calling from different threads for the same
     * thread object.
     *
     * @attention Don't call for launched and not yet joined threads.
     */
    int amp_internal_raw_thread_configure_context(struct amp_raw_thread_s *thread,
                                                  void *context);
    
    
    /**
     * Set the thread function.
     *
     * A race condition can occur if calling from different threads for the same
     * thread object.
     *
     * @attention Don't call for launched and not yet joined threads.
     */
    int amp_internal_raw_thread_configure_function(struct amp_raw_thread_s *thread,
                                                   amp_raw_thread_func_t func);
    
    /**
     * Returns the thread context in *context.
     *
     * The returned value can be invalid if the function is called while other
     * functions manipulate the thread and its fields.
     *
     * Based on the memory model the value might not be consistent between
     * threads.
     */
    int amp_internal_raw_thread_context(struct amp_raw_thread_s *thread,
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
    int amp_internal_raw_thread_function(struct amp_raw_thread_s *thread,
                                         amp_raw_thread_func_t *func);
    
    
    /**
     * @attention Don't call for launched and not yet joined threads.
     */
    int amp_internal_raw_thread_launch_configured(struct amp_raw_thread_s *thread);
    
    
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif
    

#endif /* AMP_amp_internal_raw_thread_H */

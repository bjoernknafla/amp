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
 * Token for amp_raw_thread_s->state symbolizes thread hasn't launched.
 */
#define AMP_INTERNAL_RAW_THREAD_PRELAUNCH_STATE 0x0bebe42

/**
 * Token for amp_raw_thread_s->state symbolizes thread has launched.
 */
#define AMP_INTERNAL_RAW_THREAD_LAUNCHED_STATE 0xbeeb42

/**
 * Token for amp_raw_thread_s->state symbolizes thread has joined.
 */
#define AMP_INTERNAL_RAW_THREAD_JOINED_STATE 0xebbe42



    
    /**
     * @attention Don't call for launched and not yet joined threads.
     */
    int amp_internal_raw_thread_init(struct amp_raw_thread_s *thread,
                                     void* func_context,
                                     amp_raw_thread_func_t func);
    
    
    
    /**
     * @attention Don't call for launched and not yet joined threads.
     */
    int amp_internal_raw_thread_launch_initialized(struct amp_raw_thread_s *thread);
    
    
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif
    

#endif /* AMP_amp_internal_raw_thread_H */

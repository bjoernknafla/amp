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
 * Platform dependent definition of amp_raw_barrier_s and the associated init
 * and finalize functions to enable placement of a barrier on the stack.
 */

#ifndef AMP_amp_raw_barrier_H
#define AMP_amp_raw_barrier_H

#include <amp/amp_barrier.h>



#if defined(AMP_USE_GENERIC_BROADCAST_BARRIERS) || defined(AMP_USE_GENERIC_SIGNAL_BARRIERS)
#   include <amp/amp_raw_mutex.h>
#   include <amp/amp_raw_condition_variable.h>
#else
#   error Unsupported backend.
#endif



#if defined(__cplusplus)
extern "C" {
#endif


    
    /**
     * Implementation of a platform dependent barrier to be treated as opaque
     * as its implementation can change at any time.
     */
    struct amp_raw_barrier_s {
#if defined(AMP_USE_GENERIC_BROADCAST_BARRIERS)
        struct amp_raw_mutex_s count_mutex;
        struct amp_raw_condition_variable_s waking_condition;
        
        amp_barrier_count_t count;
        amp_barrier_count_t init_count;
        amp_barrier_count_t period;
        int valid;
#elif defined(AMP_USE_GENERIC_SIGNAL_BARRIERS)
        struct amp_raw_mutex_s count_mutex;
        struct amp_raw_condition_variable_s counting_condition;
        struct amp_raw_condition_variable_s waking_condition;
        
        amp_barrier_count_t count;
        amp_barrier_count_t init_count;
        int state;
        int valid;
#else
#   error Unsupported backend.
#endif
    };

    
    int amp_raw_barrier_init(amp_barrier_t barrier,
                             amp_barrier_count_t init_count);
    
    int amp_raw_barrier_finalize(amp_barrier_t barrier);

    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif


#endif /* AMP_amp_raw_barrier_H */

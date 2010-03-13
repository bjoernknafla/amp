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

#include "amp_internal_raw_thread.h"


#include <errno.h>
#include <assert.h>


#include "amp_stddef.h"


/**
 * A Pthreads thread function that internally calls the user set 
 * amp_raw_thread_func_t function.
 *
 * Purely internal function.
 */
void* amp_internal_native_thread_adapter_func(void *thread);
void* amp_internal_native_thread_adapter_func(void *thread)
{
    struct amp_raw_thread_s *thread_context = (struct amp_raw_thread_s *)thread;
    
    /*
     * Check if this the thread the argument indicates it should be.
     * The thread id can't be tested here as it is only stored after launching
     * - creating the thread.
     */
    /* assert(0 != pthread_equal(thread_context->native_thread_description.thread , pthread_self()));*/
    
    thread_context->thread_func(thread_context->thread_func_context);
    
    return NULL;
}



int amp_internal_raw_thread_launch_initialized(struct amp_raw_thread_s *thread)
{
    assert(NULL != thread);
    assert(NULL != thread->thread_func);
    assert(AMP_INTERNAL_RAW_THREAD_PRELAUNCH_STATE == thread->state);
    
    if (   (NULL == thread) 
        || (NULL == thread->thread_func) 
        || (AMP_INTERNAL_RAW_THREAD_PRELAUNCH_STATE != thread->state)) {
        
        return EINVAL;
    }
    
    int const retval = pthread_create(&(thread->native_thread_description.thread), 
                                      NULL, /* Default thread creation attribs. */
                                      amp_internal_native_thread_adapter_func, 
                                      thread);
    assert(EINVAL != retval && "Thread attributes are invalid.");
    assert((0 == retval || EAGAIN == retval) && "Unexpected error.");
    
    if (0 == retval) {
        thread->state = AMP_INTERNAL_RAW_THREAD_LAUNCHED_STATE;
    }
    
    return retval;
}



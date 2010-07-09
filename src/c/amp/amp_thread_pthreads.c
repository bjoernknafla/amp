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
 * Shallow wrapper around Pthreads threads implements all backend specific 
 * (non-common) functions from amp_thread.h, amp_raw_thread.h, and 
 * amp_internal_thread.h.
 *
 * TODO: @todo Decide if to use Pthreads yield instead of POSIX yield.
 *
 * TODO: @todo Remove thread id hack using pthread_t and replace with a proper
 *             thread id implementation. Also look into the next C standard
 *             and the next C++ standard to get an idea how to refactor the
 *             whole id thematic. Idea: use a thread local slot that points
 *             to the threads amp struct and use that address as the id value.
 *             The main thread is represented by NULL and accessing the amp
 *             thread id on non-amp-thread-created threads is undefined 
 *             behavior. Or let one thread point to the other to organize
 *             truly unique numbers as ids though this will lead to O(n) 
 *             complexity.
 */


#include "amp_thread.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include <sched.h>

#include "amp_stddef.h"
#include "amp_return_code.h"
#include "amp_raw_thread.h"
#include "amp_internal_thread.h"



/**
 * A Pthreads thread function that internally calls the user set 
 * amp_raw_thread_func_t function.
 *
 * Purely internal function.
 */
void* amp_internal_native_thread_adapter_func(void *thread);
void* amp_internal_native_thread_adapter_func(void *thread)
{
    amp_thread_t thread_context = (amp_thread_t)thread;
    
    /*
     * Check if this is the thread the argument indicates it should be.
     * The thread id can't be tested here as it is only stored after launching
     * - creating the thread.
     */
    /* assert(0 != pthread_equal(thread_context->native_thread_description.thread , pthread_self()));*/
    
    thread_context->func(thread_context->func_context);
    
    /**
     * TODO: @todo The moment amp atomic ops are available add a way to 
     *             atomically set the thread state so it is observable by other
     *             threads.
     */
    
    return NULL;
}



int amp_internal_native_thread_set_invalid(struct amp_native_thread_s *native_thread)
{
    assert(NULL != native_thread);
    
    native_thread->thread = AMP_INTERNAL_INVALID_THREAD_ID;
    
    return AMP_SUCCESS;
}



int amp_internal_thread_launch_configured(amp_thread_t thread)
{
    assert(NULL != thread);
    assert(NULL != thread->func);
    assert(amp_internal_thread_prelaunch_state == thread->state);
    
    if ((NULL == thread->func) 
        || (amp_internal_thread_prelaunch_state != thread->state)) {
        
        return AMP_ERROR;
    }
    
    int retval = pthread_create(&(thread->native_thread_description.thread), 
                                NULL, /* Default thread creation attribs. */
                                amp_internal_native_thread_adapter_func, 
                                thread);
    if (0 == retval) {
        thread->state = amp_internal_thread_joinable_state;
    } else {
        switch (retval) {
            case EAGAIN:
                retval = AMP_ERROR;
                break;
            default: /* EINVAL, EPERM - programming error */
                assert(0);
                retval = AMP_ERROR;
        }
    }
    
    return retval;
}



int amp_raw_thread_join(amp_thread_t thread)
{
    assert(0 != thread);
    assert(amp_internal_thread_joinable_state == thread->state);
    
    if (amp_internal_thread_joinable_state != thread->state) {
        /* If thread hasn't been launched it could be already joined or is 
         * invalid.
         */
        if (amp_internal_thread_joined_state == thread->state) {
            /* Thread has already joined. */
            return AMP_ERROR    ;
        } else {
            /* thread doesn't point to valid thread data. */
            return AMP_ERROR;
        }
    }
    
    /* Currently is ignored. */
    void *thread_exit_value = 0;
    int retval = pthread_join(thread->native_thread_description.thread,
                              &thread_exit_value);
    if (0 == retval) {
        /* Successful join.*/
        thread->state = amp_internal_thread_joined_state;
    } else {
        /* EINVAL, ESRCH, EDEADLK - programming error */
        assert(0);
        retval = AMP_ERROR;
    }
    
    return retval;
}



amp_internal_thread_id_t amp_internal_thread_current_id(void)
{
    /*
     * TODO: @todo Replace hack!
     */
    return (amp_internal_thread_id_t)pthread_self();
}



int amp_internal_thread_id(amp_thread_t thread,
                  amp_internal_thread_id_t *id)
{
    assert(NULL != thread);
    assert(NULL != id);
    
    if (amp_internal_thread_joinable_state != thread->state) {
        *id = AMP_INTERNAL_INVALID_THREAD_ID;
        return AMP_ERROR;
    }
    
    *id = (amp_internal_thread_id_t)(thread->native_thread_description.thread);
    
    return AMP_SUCCESS;
}



int amp_thread_yield(void)
{
    return sched_yield();
}



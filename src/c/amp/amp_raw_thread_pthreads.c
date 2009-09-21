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
 * @file
 *
 * Shallow wrapper around Pthreads threads.
 */


#include "amp_raw_thread.h"


/* Include assert */
#include <assert.h>

/* Include EINVAL, ESRCH, EAGAIN, EDEADLK */
#include <errno.h>



/* Include sched_yield */
#include <sched.h>




/**
 * Token for amp_raw_thread_s->state symbolizes thread hasn't launched.
 */
#define AMP_RAW_THREAD_PRELAUNCH_STATE 0x0bebe42

/**
 * Token for amp_raw_thread_s->state symbolizes thread has launched.
 */
#define AMP_RAW_THREAD_LAUNCHED_STATE 0xbeeb42

/**
 * Token for amp_raw_thread_s->state symbolizes thread has joined.
 */
#define AMP_RAW_THREAD_JOINED_STATE 0xebbe42


/**
 * A platforms thread function that internally calls the user set 
 * amp_raw_thread_func_t function.
 * Purely internal function.
 */
void* native_thread_adapter_func(void *thread);
void* native_thread_adapter_func(void *thread)
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



int amp_raw_thread_launch(amp_raw_thread_t *thread, 
                           void *thread_func_context, 
                           amp_raw_thread_func_t thread_func)
{
    assert(NULL != thread);
    assert(NULL != thread_func);
    
    if (NULL == thread || NULL == thread_func) {
        return EINVAL;
    }
    
    thread->thread_func = thread_func;
    thread->thread_func_context = thread_func_context;
    thread->state = AMP_RAW_THREAD_PRELAUNCH_STATE;
    
    int const retval = pthread_create(&(thread->native_thread_description.thread), 
                                      NULL, /* Default thread creation attribs. */
                                      native_thread_adapter_func, 
                                      thread);
    assert(EINVAL != retval && "Thread attributes are invalid.");
    assert((0 == retval || EAGAIN == retval) && "Unexpected error.");
    
    if (0 == retval) {
        thread->state = AMP_RAW_THREAD_LAUNCHED_STATE;
    }
    
    return retval;
}



int amp_raw_thread_join(amp_raw_thread_t *thread)
{
    assert(0 != thread);
    assert(AMP_RAW_THREAD_LAUNCHED_STATE == thread->state);
    
    if (AMP_RAW_THREAD_LAUNCHED_STATE != thread->state) {
        /* If thread hasn't been launched it could be already joined or is 
         * invalid.
         */
        if (AMP_RAW_THREAD_JOINED_STATE == thread->state) {
            /* Thread has already joined. */
            return EINVAL;
        } else {
            /* thread doesn't point to valid thread data. */
            return ESRCH;
        }
    }
    
    /* Currently is ignored. */
    void *thread_exit_value = 0;
    int const retval = pthread_join(thread->native_thread_description.thread,
                                    &thread_exit_value);
    assert( (0 == retval 
             || EINVAL == retval 
             || EDEADLK == retval 
             || ESRCH == retval) 
           && "Unexpected error.");
    
    if (0 == retval) {
        /* Successful join.*/
        thread->state = AMP_RAW_THREAD_JOINED_STATE;
    }
    
    return retval;
}




amp_raw_thread_id_t amp_raw_thread_get_id(void)
{
    /*
     * TODO: @todo Remove hack! Most Pthreads libraries implement pthread_t as
     *             a pointer to a thread struct, a pointeter can be converted to
     *             
     */
    return (amp_raw_thread_id_t)pthread_self();
}



int amp_raw_thread_yield(void)
{
    return sched_yield();
}



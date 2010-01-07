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
 * Shallow wrapper around Windows threads.
 *
 * In VC compile with /MT
 */


#include "amp_raw_thread.h"


#include "amp_internal_raw_thread.h"


/* Include assert */
#include <assert.h>

/* Include EINVAL, ESRCH, EAGAIN, EDEADLK */
#include <errno.h>


/* Include _eginthreadex, _endthreadex */
#include <process.h>



/* Include AMP_SUCCESS */
#include "amp_stddef.h"



/**
 * TODO: @todo Add better error detection and handling.
 */
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
    
    /* TODO: @todo If WaitForSingleObject detects this error condition itself
     * don't handle it beforehand.
     */
    if (thread->native_thread_description.thread_id == GetCurrentThreadId()) {
        assert(thread->native_thread_description.thread_id != GetCurrentThreadId()
               && "Calling thread tries to join with itself.");
        
        return EDEADLK;
    }
    
    int retval = AMP_SUCCESS;
    /* TODO: @todo Can this detect waiting on its own thread? */
    DWORD const wait_retval = WaitForSingleObject(thread->native_thread_description.thread_handle,
                                                  INFINITE);
    
    if (WAIT_OBJECT_0 == wait_retval) {
        
        /* Currently unused. */
        DWORD exit_code = 0;
        BOOL const getexitcode_retval = GetExitCodeThread(thread->native_thread_description.thread_handle,
                                                         &exit_code);
        /* TODO: @todo Add better error detection and handling. */
        assert(TRUE == getexitcode_retval && "Unexpected error.");
        assert(STILL_ACTIVE != exit_code && "Thread didn't end.");
        
        
        BOOL const close_handle_retval = CloseHandle(thread->native_thread_description.thread_handle);
        if ( TRUE == close_handle_retval) {
            thread->state = AMP_RAW_THREAD_JOINED_STATE;
            retval = AMP_SUCCESS;
        } else {
            /* If waiting on the thread was successful this shouldn't happen. */
            /* DWORD const last_error = GetLastError(); */

            assert(false && "Unknown error.");
            
            retval = EINVAL;
        }
    } else {
        /* An error occured - handle seems to be invalid, e.g. already closed */
        assert(WAIT_TIMEOUT == wait_retval && "INFINITE can't timeout.");
        
        /* DWORD const last_error = GetLastError(); */
        
        assert(false && "Unknown error.");
        
        retval = EINVAL;
        
    }

    assert(AMP_SUCCESS == retval && "Unexpected error.");
    
    
    return retval;
}



amp_raw_thread_id_t amp_raw_thread_id(void)
{
    /*
     * TODO: @todo Remove hack! Most Pthreads libraries implement pthread_t as
     *             a pointer to a thread struct, a pointeter can be converted to
     *             
     */
    return (amp_raw_thread_id_t)GetCurrentThreadId();
}



amp_raw_thread_id_t amp_raw_thread_get_id(amp_raw_thread_t *thread)
{
    assert(NULL != thread);
    
    if (AMP_RAW_THREAD_LAUNCHED_STATE != thread->state) {
        return 0;
    }
    
    return (amp_raw_thread_id_t)(thread->native_thread_description.thread_id);
}



int amp_raw_thread_yield(void)
{
    /* Ignore return code as it only indicates if there are other threads
     * to switch to or not, but @em amp uses this function only as a hint.
     */
    SwitchToThread();
    
    return AMP_SUCCESS;
}




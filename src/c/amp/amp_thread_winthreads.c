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
 * Shallow wrapper around Windows threads implements all backend specific 
 * (non-common) functions from amp_thread.h, amp_raw_thread.h, and 
 * amp_internal_thread.h.
 *
 * In VC compile with option /MT
 */


#include "amp_thread.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include <process.h>

#include "amp_stddef.h"
#include "amp_return_code.h"
#include "amp_raw_thread.h"
#include "amp_internal_thread.h"



/**
 * A platforms thread function that internally calls the user set 
 * amp_raw_thread_func_t function.
 * Purely internal function.
 * Native code (not managed code).
 */
unsigned int __stdcall native_thread_adapter_func(void *thread);
unsigned int __stdcall native_thread_adapter_func(void *thread)
{
    amp_thread_t thread_context = (amp_thread_t)thread;
    
    /*
     * Check if this the thread the argument indicates it should be.
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
    
    /* Return meaningless return code. */
    return 0;
}



int amp_internal_native_thread_set_invalid(struct amp_native_thread_s *native_thread)
{
    assert(NULL != native_thread);
    
    native_thread->thread_handle = AMP_INTERNAL_INVALID_THREAD_ID;
    native_thread->thread_id = AMP_INTERNAL_INVALID_THREAD_ID;
    
    return AMP_SUCCESS;
}



int amp_internal_thread_launch_configured(amp_thread_t thread)
{
    unsigned int inter_process_thread_id = 0;
    uintptr_t thread_handle = 0;
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != thread);
    assert(NULL != thread->func);
    assert(amp_internal_thread_prelaunch_state == thread->state);
    
    if ((NULL == thread) 
        || (NULL == thread->func) 
        || (amp_internal_thread_prelaunch_state != thread->state)) {
        
        return EINVAL;
    }
    
    /* Thread creation for native code. */
    errno = 0;
    thread_handle = _beginthreadex(NULL, /* Non-inheritable security attribs. */
                                   0,  /* Default thread stack size. */
                                   native_thread_adapter_func, 
                                   thread, 
                                   0, /* Start thread running. */
                                   &inter_process_thread_id);
    if (0 != thread_handle) {
        /* Thread launched successfully. */
        thread->native_thread_description.thread_handle = (HANDLE) thread_handle;
        thread->native_thread_description.thread_id = (DWORD) inter_process_thread_id;
        thread->state = amp_internal_thread_joinable_state;
        
        retval = AMP_SUCCESS;
    } else {
        /* Error while launching thread. */
        
        int const error_code = errno;
        
        switch (error_code) {
            case EAGAIN: /* Too many threads - might only be valid for _beginthread */
                retval = AMP_ERROR;
                break;
            default: /* EINVAL - programming error */
                assert(0);
                retval = AMP_ERROR;
        }
    }
    
    return retval;
}



/**
 * TODO: @todo Add better error detection and handling.
 */
int amp_raw_thread_join(amp_thread_t thread)
{
    int retval = AMP_UNSUPPORTED;
    
    assert(0 != thread);
    assert(amp_internal_thread_joinable_state == thread->state);
    
    if (amp_internal_thread_joinable_state != thread->state) {
        /* If thread hasn't been launched it could be already joined or is 
         * invalid.
         */
        if (amp_internal_thread_joined_state == thread->state) {
            /* Thread has already joined. */
            return AMP_ERROR;
        } else {
            /* thread doesn't point to valid thread data. */
            return AMP_ERROR;
        }
    }
    
    /* Calling thread tries to join with itself.
     * TODO: @todo If WaitForSingleObject detects this error condition itself
     *             don't handle it beforehand.
     */
    if (thread->native_thread_description.thread_id == GetCurrentThreadId()) {
        assert(thread->native_thread_description.thread_id != GetCurrentThreadId());
        
        return AMP_ERROR;
    }
    
    /* TODO: @todo Can this detect waiting on its own thread? */
    if (WAIT_OBJECT_0 == WaitForSingleObject(thread->native_thread_description.thread_handle,
                                             INFINITE)) {
        
        /* Currently unused. */
        DWORD exit_code = 0;
        BOOL close_handle_retval = FALSE;
        BOOL const getexitcode_retval = GetExitCodeThread(thread->native_thread_description.thread_handle,
                                                         &exit_code);
        if (FALSE == getexitcode_retval
            || STILL_ACTIVE == exit_code) {
            
            /* TODO: @todo Add better error detection and handling. */
            assert(TRUE == getexitcode_retval && "Unexpected error.");
            assert(STILL_ACTIVE != exit_code && "Thread didn't end.");
            
            return AMP_ERROR;
        }
        
        close_handle_retval = CloseHandle(thread->native_thread_description.thread_handle);
        if ( TRUE == close_handle_retval) {
            thread->state = amp_internal_thread_joined_state;
            retval = AMP_SUCCESS;
        } else {
            /* If waiting on the thread was successful this shouldn't happen. */
            DWORD const last_error = GetLastError();
            assert(0);
            
            retval = AMP_ERROR;
        }
    } else {
        /* Programming error */
        DWORD const last_error = GetLastError();
        assert(0);
        
        retval = AMP_ERROR;
    }
    
    return retval;
}



amp_internal_thread_id_t amp_internal_thread_current_id(void)
{
    return (amp_internal_thread_id_t)GetCurrentThreadId();
}



int amp_internal_thread_id(amp_thread_t thread,
                  amp_internal_thread_id_t* id)
{
    assert(NULL != thread);
    assert(NULL != id);
    
    if (amp_internal_thread_joinable_state != thread->state) {
        *id = AMP_INTERNAL_INVALID_THREAD_ID;
        return AMP_ERROR;
    }
    
    *id = (amp_internal_thread_id_t)(thread->native_thread_description.thread_id);
    
    return AMP_SUCCESS;
}



int amp_thread_yield(void)
{
    /* Ignore return code as it only indicates if there are other threads
     * to switch to or not, but @em amp uses this function only as a hint.
     */
    BOOL const switched_to_another_thread = SwitchToThread();
    (void)switched_to_another_thread;
    
    return AMP_SUCCESS;
}




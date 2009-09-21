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


/* Include assert */
#include <assert.h>

/* Include EINVAL, ESRCH, EAGAIN, EDEADLK */
#include <errno.h>


/* Include _eginthreadex, _endthreadex */
#include <process.h>



/* Include AMP_SUCCESS */
#include "amp_stddef.h"



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
 * Native code (not managed code).
 */
unsigned int __stdcall native_thread_adapter_func(void *thread);
unsigned int __stdcall native_thread_adapter_func(void *thread)
{
    struct amp_raw_thread_s *thread_context = (struct amp_raw_thread_s *)thread;
    
    /*
     * Check if this the thread the argument indicates it should be.
     * The thread id can't be tested here as it is only stored after launching
     * - creating the thread.
     */
    /* assert(0 != pthread_equal(thread_context->native_thread_description.thread , pthread_self()));*/
    
    thread_context->thread_func(thread_context->thread_func_context);
    
    /* Return meaningless return code. */
    return 0;
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
    

    /* Thread creation for native code. */
    unsigned int inter_process_thread_id = 0;
    uintptr_t thread_handle = _beginthreadex(NULL, /* Non-inheritable security attribs. */
                                             0,  /* Default thread stack size. */
                                             native_thread_adapter_func, 
                                             thread, 
                                             0, /* Start thread running. */
                                             &inter_process_thread_id);
    int retval = AMP_SUCCESS;
    if (0 != thread_handle) {
        /* Thread launched successfully. */
        thread->native_thread_description.thread_handle = (HANDLE) thread_handle;
        thread->native_thread_description.thread_id = (DWORD) inter_process_thread_id;
        thread->state = AMP_RAW_THREAD_LAUNCHED_STATE;
        
        retval = AMP_SUCCESS;
    } else {
        /* Error while launching thread. */
        
        int const error_code = errno;
        
        switch (error_code) {
            case EAGAIN:
                retval = EAGAIN;
                break;
            case EINVAL:
                assert(false && "Unexpected error.");
                break;
            case EACCES:
                retval = EAGAIN;
                break;
            default:
                assert(false && "Unknown error.");
                retval = EINVAL;
        }
    }
    
    assert(AMP_SUCCESS == retval || EAGAIN == retval && "Unexpected error.");

    
    return retval;
}


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


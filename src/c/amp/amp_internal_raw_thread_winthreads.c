/*
 *  amp_internal_raw_thread_winthreads.c
 *  amp
 *
 *  Created by Björn Knafla on 07.01.10.
 *  Copyright 2010 Bjoern Knafla Parallelization + AI + Gamedev Consulting. All rights reserved.
 *
 */

#include "amp_internal_raw_thread.h"


#include <errno.h>
#include <assert.h>


#include "amp_stddef.h"



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



int amp_internal_raw_thread_launch_initialized(struct amp_raw_thread_s *thread)
{
    assert(NULL != thread);
    assert(NULL != thread->thread_func);
    assert(AMP_INTERNAL_RAW_THREAD_PRELAUNCH_STATE == thread->state);
    
    if ((NULL == thread) 
        || (NULL == thread->thread_func) 
        || (AMP_INTERNAL_RAW_THREAD_PRELAUNCH_STATE != thread->state)) {
        
        return EINVAL;
    }
    
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
    
    assert((AMP_SUCCESS == retval || EAGAIN == retval) && "Unexpected error.");
    
    
    return retval;
}



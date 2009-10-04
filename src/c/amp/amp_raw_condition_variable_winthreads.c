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
 * Implementation of POSIX thread condition variable alikes using Windows
 * CRITICAL_SECTION, events, event waiting and signaling, and semaphores.
 *
 * See http://www.cse.wustl.edu/~schmidt/win32-cv-1.html for infos about
 * possible different implementation techniques but beware - there are bugs
 * in all so called "solutions" shown.
 */


#include "amp_raw_condition_variable.h"


/* Include LONG_MAX */
#include <limits.h>


#include "amp_internal_winthreads_critical_section_config.h"



int amp_raw_condition_variable_init(amp_raw_condition_variable_t cond)
{
    assert(NULL != cond);
    
    if (NULL == cond) {
        return EINVAL;
    }
    
    
    BOOL retval = InitializeCriticalSectionAndSpinCount(&cond->wake_waiting_threads_critsec,
                                                        AMP_RAW_MUTEX_WINTHREADS_CRITICAL_SECTION_DEFAULT_SPIN_COUNT | AMP_RAW_MUTEX_WINTHREADS_CRITICAL_SECTION_CREATE_IMMEDIATELY_ON_WIN2000);
    if (FALSE == retval) {
        /* GetLastError has more infos. */
        return ENOMEM;
    }
    
    retval = InitializeCriticalSectionAndSpinCount(&cond->access_waiting_threads_count_critsec,
                                                   AMP_RAW_MUTEX_WINTHREADS_CRITICAL_SECTION_DEFAULT_SPIN_COUNT | AMP_RAW_MUTEX_WINTHREADS_CRITICAL_SECTION_CREATE_IMMEDIATELY_ON_WIN2000);
    
    if (FALSE == retval) {
        DeleteCriticalSection(&cond->wake_waiting_threads_critsec);
        
        return ENOMEM;
    }
    
    
    
    cond->waking_waiting_threads_count_control_sem = CreateSemaphore(NULL, /* No inheritance to child processes */
                                                                     0, /* Initially no threads can pass */
                                                                     LONG_MAX, /* Max semaphore count */
                                                                     NULL); /* Only intra-process semaphore */
    
    if (NULL == cond->waking_waiting_threads_count_control_sem) {
        DeleteCriticalSection(&cond->access_waiting_threads_count_critsec);
        DeleteCriticalSection(&cond->wake_waiting_threads_critsec);
        
        DWORD const last_error = GetLastError();
        
        int ret_error_code = AMP_SUCCESS;
        
        switch (last_error) {
            case ERROR_TOO_MANY_SEMAPHORES:
                ret_error_code = EAGAIN;
                break;
            default:
                /* TODO: @todo Check which code to use to flag an unknown error. 
                 */
                assert(false && "Unknown error.");
                ret_error_code = EINVAL;
                break;
        }
        
        return ret_error_code;
    }
    
    
    
    
    
    cond->finished_waking_waiting_threads_event = CreateEvent(NULL, /* Default security and no inheritance to child processes */
                                                              FALSE, /* No manual reset */
                                                              0, /* Initially not signaled */
                                                              NULL /* Not inter-process available */
                                                              );
    
    if (NULL == cond->finished_waking_waiting_threads_event) {
        
        DWORD const create_event_error = GetLastError();
        
        DeleteCriticalSection(&cond->access_waiting_threads_count_critsec);
        DeleteCriticalSection(&cond->wake_waiting_threads_critsec);

        BOOL const close_retval = CloseHandle(&cond->waking_waiting_threads_count_control_sem);
        
        int ret_error_code = AMP_SUCCESS;
        
        if (FALSE == close_retval) {
            DWORD const close_handle_error = GetLastError();
            
            switch (close_handle_error) {
                case ERROR_SEM_IS_SET:
                    /* TODO: @todo Need a Windows dev expert to check if this is the 
                     * right error code interpretation. 
                     */
                    assert(false && "Semaphore is in use.");
                    
                    ret_error_code = EBUSY;
                    break;
                case ERROR_SEM_OWNER_DIED:
                    /* TODO: @todo Check if this error can really happen. */
                    assert(false 
                           && "It shouldn't happen that the previous ownership of the semaphore has ended.");
                    ret_error_code = EINVAL;
                    break;
                case ERROR_SEM_NOT_FOUND:
                    assert(false && "It shouldn't happen that the (not) specified system semaphore name wasn't found.");
                    ret_error_code = EINVAL;
                    break;
                default:
                    /* TODO: @todo Check which code to use to flag an unknown error. 
                     */
                    assert(false && "Unknown error.");
                    ret_error_code = EINVAL;
                    break;
            }
            
            return ret_error_code;
        }
        
        /* I don't know the possible return values of GetLastError if event
         * creation didn't work - just returning an error code.
         */
        return EAGAIN;
    }
    
    
    cond->waiting_thread_count = 0l;
    
    return AMP_SUCCESS;
}



int amp_raw_condition_variable_finalize(amp_raw_condition_variable_t cond)
{
    assert(NULL != cond);
    
    if (NULL == cond) {
        return EINVAL;
    }
    
    
    DeleteCriticalSection(&cond->access_waiting_threads_count_critsec);
    DeleteCriticalSection(&cond->wake_waiting_threads_critsec);
    
    BOOL const close_sem = CloseHandle(&cond->waking_waiting_threads_count_control_sem);
    DWORD const close_sem_error = GetLastError();
    BOOL const close_event = CloseHandle(&cond->finished_waking_waiting_threads_event);
    DWORD const close_event_error = GetLastError();
    
    int ret_error_code = AMP_SUCCESS;
    
    /* If both handle closing functions return an error the semaphore error will
     * hide the event error.
     */
    if (FALSE == close_event) {
        /*
         * TODO: @todo Find out which error codes GetLastError returns when
         *             closing and event handler fails.
         */
        
        ret_error_code = EINVAL;
    }
    
    if (FALSE == close_sem) {
        switch (close_sem_error) {
            case ERROR_SEM_IS_SET:
                /* TODO: @todo Need a Windows dev expert to check if this is the 
                 * right error code interpretation. 
                 */
                assert(false && "Semaphore is in use.");
                
                ret_error_code = EBUSY;
                break;
            case ERROR_SEM_OWNER_DIED:
                /* TODO: @todo Check if this error can really happen. */
                assert(false 
                       && "It shouldn't happen that the previous ownership of the semaphore has ended.");
                ret_error_code = EINVAL;
                break;
            case ERROR_SEM_NOT_FOUND:
                assert(false && "It shouldn't happen that the (not) specified system semaphore name wasn't found.");
                ret_error_code = EINVAL;
                break;
            default:
                /* TODO: @todo Check which code to use to flag an unknown error. 
                 */
                assert(false && "Unknown error.");
                ret_error_code = EINVAL;
                break;
        }
    }
        
    return ret_error_code;
}



int amp_raw_condition_variable_broadcast(amp_raw_condition_variable_t cond)
{
    
}



int amp_raw_condition_variable_signal(amp_raw_condition_variable_t cond)
{
    
}



int amp_raw_condition_variable_wait(amp_raw_condition_variable_t cond,
                                    amp_raw_mutex_t mutex)
{
    
}



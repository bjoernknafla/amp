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
 *
 * See http://www.opengroup.org/onlinepubs/000095399/functions/pthread_cond_timedwait.html
 * for more infos about the POSIX threads condition variable specification.
 *
 * TODO: @todo Check if SEH (Windows structured exception handling) should be
 *             added.
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
    
    

    
    retval = InitializeCriticalSectionAndSpinCount(&cond->access_waiting_threads_count_critsec,
                                                   AMP_RAW_MUTEX_WINTHREADS_CRITICAL_SECTION_DEFAULT_SPIN_COUNT | AMP_RAW_MUTEX_WINTHREADS_CRITICAL_SECTION_CREATE_IMMEDIATELY_ON_WIN2000);
    
    if (FALSE == retval) {
        
        
        return ENOMEM;
    }
    

    
    cond->wake_waiting_threads_mutex = CreateMutex(NULL, /* Don't inherit to child processes */
                                                   FALSE, /* Creating thread doesn't own the mutex without locking it beforehand */
                                                   NULL /* No name, mutex only used within one process */
                                                   );
    if (NULL  == cond->wake_waiting_threads_mutex) {
        /* GetLastError has more infos. */
        DeleteCriticalSection(&cond->access_waiting_threads_count_critsec);
        
        return EAGAIN;
    }
    
    
    
    
    /* Assuming that less threads exist than max possible semaphore count.
     */
    cond->waking_waiting_threads_count_control_sem = CreateSemaphore(NULL, /* No inheritance to child processes */
                                                                     0, /* Initially no threads can pass */
                                                                     LONG_MAX, /* Max semaphore count */
                                                                     NULL); /* Only intra-process semaphore */
    
    if (NULL == cond->waking_waiting_threads_count_control_sem) {
        DeleteCriticalSection(&cond->access_waiting_threads_count_critsec);
        
        if (FALSE == CloseHandle(cond->wake_waiting_threads_mutex)) {
            /* TODO: @todo Decide which error code to return. */
            return EINVAL;
        }
        
        
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
        
        BOOL const close_mutex_retval = CloseHandle(cond->wake_waiting_threads_mutex);

        BOOL const close_retval = CloseHandle(&cond->waking_waiting_threads_count_control_sem);
        
        
        if (FALSE == close_mutex_retval) {
            /* TODO: @todo Decide for a better way to handle multiple errors and
             *             which error code to return.
             */
            
            return EINVAL;
        }
        
        
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
    cond->broadcast_in_progress = FALSE;
    
    
    /* Preliminary tests that waiting_thread_count and broadcast_in_progress
     * are correctly aligned to allow atomic access to them.
     */
    assert(0x0 == ((&cond->waiting_thread_count) & 0xf));
    assert(0x0 ==((&cond->broadcast_in_progress) & 0xf));
    
    return AMP_SUCCESS;
}



int amp_raw_condition_variable_finalize(amp_raw_condition_variable_t cond)
{
    assert(NULL != cond);
    
    if (NULL == cond) {
        return EINVAL;
    }
    
    
    DeleteCriticalSection(&cond->access_waiting_threads_count_critsec);
    
    BOOL const close_mutex = CloseHandle(cond->wake_waiting_threads_mutex);
    DWORD const close_mutex_error = GetLastError();
    BOOL const close_sem = CloseHandle(cond->waking_waiting_threads_count_control_sem);
    DWORD const close_sem_error = GetLastError();
    BOOL const close_event = CloseHandle(cond->finished_waking_waiting_threads_event);
    DWORD const close_event_error = GetLastError();
    
    int ret_error_code = AMP_SUCCESS;
    
    /* If multiple handle closing functions return an error the mutex error 
     * will be hidden by the other errors.
     */
    if (FALSE == close_mutex) {
        /*
         * TODO: @todo Find out which error codes GetLastError returns when
         *             closing and event handler fails.
         */
        
        ret_error_code = EINVAL;
    }
    
    /* If multiple handle closing functions return an error the semaphore error 
     * will hide the event error.
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
    assert(NULL != cond);
    
    DWORD const lock_retval = WaitForSingleObject(cond->wake_waiting_threads_mutex, 
                                                  INFINITE);
#if !defined(NDEBUG)
    assert(WAIT_OBJECT_0 == lock_retval);
    if (lock_retval != WAIT_OBJECT_0) {
        /* Possible errors: mutex was abandoned or hasn't been initialized.
         */
        
        return EINVAL;
    }
#endif
    
    
    LONG const waiting_thread_count =  cond->waiting_thread_count;
    
    if (0 < waiting_thread_count) {
        
        cond->broadcast_in_progress = TRUE;
        /* Releasing the sem here and waiting on it should update the memory of 
         * the waiting threads to see that a broadcast is in progress.
         */
        LONG prev_sem_count = 0;
        /* Assuming that less threads exist than max possible semaphore count.
         * TODO: @todo Decide if to spin here if the assumption doesn't hold
         *             true in the future?
         */
        assert(waiting_thread_count <= LONG_MAX 
               && "Assuming that less threads exist than max possible semaphore count.");
        BOOL const release_retval = ReleaseSemaphore(cond->waking_waiting_threads_count_control_sem,
                                                     waiting_thread_count,
                                                     &prev_sem_count /* No interest in the previous sem count. */
                                                     );
#if !defined(NDEBUG)
        assert(prev_sem_count == waiting_thread_count);
        assert(TRUE == release_retval);
        if (FALSE == release_retval) {
            cond->broadcast_in_progress = FALSE;
            BOOL const release_mutex_retval = ReleaseMutex(cond->wake_waiting_thread_mutex);
            /* Release won't return with an error as the mutex is owned by this 
             * thread
             */
            assert(TRUE == release_mutex_retval); 
            
            return EINVAL;
        }
#endif
        
        BOOL const wait_retval = WaitForSingleObject(cond->waking_waiting_threads_count_control_sem,
                                                     INFINITE);
#if !defined(NDEBUG)
        assert(TRUE == wait_retval);
        if (FALSE == wait_retval) {
            cond->broadcast_in_progress = FALSE;
            BOOL const release_mutex_retval = ReleaseMutex(cond->wake_waiting_thread_mutex);
            /* Release won't return with an error as the mutex is owned by this 
             * thread
             */
            assert(TRUE == release_mutex_retval);
            
            return EINVAL;
        }
#endif
        cond->broadcast_in_progress = FALSE;
        
    }
    
    BOOL const release_mutex_retval = ReleaseMutex(cond->wake_waiting_thread_mutex);
    /* Release won't return with an error as the mutex is owned by this 
     * thread
     */
    assert(TRUE == release_mutex_retval);
    
    return AMP_SUCCESS;
}



int amp_raw_condition_variable_signal(amp_raw_condition_variable_t cond)
{
    assert(NULL != cond);

    DWORD const lock_retval = WaitForSingleObject(cond->wake_waiting_threads_mutex, 
                                                  INFINITE);
#if !defined(NDEBUG)
    assert(WAIT_OBJECT_0 == lock_retval);
    if (lock_retval != WAIT_OBJECT_0) {
        /* Possible errors: mutex got abandoned or hasn't been initialized. */
        
        return EINVAL;
    }
#endif
    
    BOOL at_least_one_waiting_thread = (0l != cond->waiting_thread_count);
    
    if (at_least_one_waiting_thread) {
        LONG prev_sem_count = 0;
        /* Assuming that less threads exist than max possible semaphore count.
         * TODO: @todo Decide if to spin here if the assumption doesn't hold
         *             true in the future?
         */
        BOOL const release_retval = ReleaseSemaphore(cond->waking_waiting_threads_count_control_sem, 
                                                     1, 
                                                     &prev_sem_count /* No interest in the previous sem count. */
                                                     );
#if !defined(NDEBUG)
        assert(prev_sem_count > 0);
        assert(TRUE == release_retval);
        if (FALSE == release_retval) {
            BOOL const release_mutex_retval = ReleaseMutex(cond->wake_waiting_thread_mutex);
            /* Release won't return with an error as the mutex is owned by this 
             * thread
             */
            assert(TRUE == release_mutex_retval);
            
            return EINVAL;
        }
#endif
        
        BOOL const wait_retval = WaitForSingleObject(cond->waking_waiting_threads_count_control_sem,
                                                     INFINITE);
#if !defined(NDEBUG)
        assert(TRUE == wait_retval);
        if (FALSE == wait_retval) {
            BOOL const release_mutex_retval = ReleaseMutex(cond->wake_waiting_thread_mutex);
            /* Release won't return with an error as the mutex is owned by this 
             * thread
             */
            assert(TRUE == release_mutex_retval);
            
            return EINVAL;
        }
#endif
    }
    
    BOOL const release_mutex_retval = ReleaseMutex(cond->wake_waiting_thread_mutex);
    /* Release won't return with an error as the mutex is owned by this 
     * thread
     */
    assert(TRUE == release_mutex_retval);
    
    return AMP_SUCCESS;
}



int amp_raw_condition_variable_wait(amp_raw_condition_variable_t cond,
                                    amp_raw_mutex_t mutex)
{
    /*
     * TODO: @todo Rewrite this function to prevent duplication of error 
     *             handling code. Use goto or nested if-else-branches?
     */
     
    assert(NULL != cond);
    assert(NULL != mutex);
    
    /* Get the lock that controls that threads can only add themselves to 
     * the waiting count as long as no other thread is doing so, or as long as
     * broadcast or signal aren't waiting on all previously waiting threads
     * to be woken.
     *
     * TODO: @todo Decide if to use an atomic increment instruction, possibly 
     *             with aquire semantics to prevent instruction reordering 
     *             inside the function.
     *             The critical section needs to stay though, to coordinate with
     *             signal and broadcast which can't allow new waiters to add
     *             themselves until all previously waiting threads are awake.
     */
    DWORD const lock_retval = WaitForSingleObject(cond->wake_waiting_threads_mutex, 
                                                  INFINITE);
#if !defined(NDEBUG)
    assert(WAIT_OBJECT_0 == lock_retval);
    if (lock_retval != WAIT_OBJECT_0) {
        /* Possible errors: mutex was abandoned or wasn't correctly initialized
         */
        
        return EINVAL;
    }
#endif
    {
        ++(cond->waiting_thread_count);
    }
    /* Not calling release for the mutex here to allow atomically releasing it
     * and waiting on the semaphore that controls how many threads are waking 
     * up.
     */
    /* BOOL const release_mutex_retval = ReleaseMutex(cond->wake_waiting_thread_mutex); */
    /* Release won't return with an error as the mutex is owned by this 
     * thread
     */
    /* assert(TRUE == release_mutex_retval); */
    
    
    /* Unlock the mutex to allow other threads to add themselves to the waiting
     * count or to allow broadcast or signal to be called while the mutex
     * is locked.
     * Must be done before waiting on the semaphore that control how many
     * threads are awoken so no deadlock occurs.
     *
     * Current mutex implementation should assert in debug mode and not
     * return any error in non-debug mode.
     */
    int retval = amp_raw_mutex_unlock(mutex);
#if !defined(NDEBUG)
    assert(EINVAL != retval && "Mutex is invalid.");
    assert(EPERM != retval && "Mutex is owned by another thread.");
    assert(AMP_SUCCESS == retval && "Unexpected error.");
    if (AMP_SUCCESS != retval) {
        /* EnterCriticalSection(&cond->wake_waiting_threads_critsec); Haven't released the mutex yet, so acquiring it isn't necessary */
        {
            --(cond->waiting_thread_count);
        }
        /* LeaveCriticalSection(&cond->wake_waiting_threads_critsec); */
        BOOL const release_mutex_retval = ReleaseMutex(cond->wake_waiting_thread_mutex);
        /* Release won't return with an error as the mutex is owned by this 
         * thread
         */
        assert(TRUE == release_mutex_retval);
        
        /* EINVAL is returned to signal different errors, e.g. not EPERM. */
        return EINVAL;
    }
#endif
    
    /* Assuming that less threads exist than max possible semaphore count.
     * TODO: @todo Decide if to spin here if the assumption doesn't hold
     *             true in the future?
     */
    DWORD const signal_and_wait_retval = SignalObjectAndWait(cond->wake_waiting_thread_mutex,
                                                             cond->waking_waiting_threads_count_control_sem, 
                                                             INFINITE,
                                                             FALSE /* Non io alertable */
                                                             );
#if !defined(NDEBUG)
    assert(WAIT_ABANDONED != signal_and_wait_retval 
           && "A thread owning the mutex terminated without releasing it.");
    assert(WAIT_OBJECT_0 == signal_and_wait_retval);
    if (WAIT_OBJECT_0 != signal_and_wait_retval) {
        /* Mutex was acquiered from within this function therefore won't produce
         * an error. Problem must be semaphore related...
         */
        
        DWORD const lock_retval = WaitForSingleObject(cond->wake_waiting_threads_mutex, 
                                                      INFINITE);
        assert(WAIT_OBJECT_0 == lock_retval);
        {
            --(cond->waiting_thread_count);
        }
        BOOL const release_mutex_retval = ReleaseMutex(cond->wake_waiting_thread_mutex);
        assert(TRUE == release_mutex_retval);
        
        int retval = amp_raw_mutex_lock(mutex);
        /* Error would surface earlier */
        assert(EINVAL != retval && "Mutex is invalid.");
        /* Error would surface earlier */
        assert(EDEADLK != retval && "Mutex is already locked by this thread.");
        assert(AMP_SUCCESS == retval && "Unexpected error.");
        
        return EINVAL;
    }
#endif
    
    /* TODO: @todo Remove code in comment. */
    /* DWORD const sem_wait_retval = WaitForSingleSemaphore(cond->waking_waiting_threads_count_control_sem, 
                           INFINITE); */
    /* assert(WAIT_OBJECT_0 == sem_wait_retval); */
    /* TODO: @todo Decide if to really handle this error. Based on MSDN unsure
     *             if it can really happen.
     */
    /* if (WAIT_OBJECT_0 != sem_wait_retval) {
        EnterCriticalSection(&cond->wake_waiting_threads_critsec);
        {
            --(cond->waiting_thread_count);
        }
        LeaveCriticalSection(&cond->wake_waiting_threads_critsec);*/
        
        /* Lock the mutex before handing back control to the caller.
         *
         * Current mutex implementation should assert in debug mode and not
         * return any error in non-debug mode.
         */
        /* int retval = amp_raw_mutex_lock(mutex); */
        /* Error would surface earlier */
        /* assert(EINVAL != retval && "Mutex is invalid."); */
        /* Error would surface earlier */
        /* assert(EDEADLK != retval && "Mutex is already locked by this thread."); */
        /* assert(AMP_SUCCESS == retval && "Unexpected error."); */
        
        /* return EINVAL; */
    /* } */
    
    
    
    
    /* Control and synchronize access to the waiting thread counter. It is only
     * accessed from awoken waiting threads.
     *
     * TODO: @todo Decide if to replace the critical section by using 
     *             an atomic decrement instruction, possibly with release
     *             semantics to prevent instruction reordering inside the
     *             function. Then atomic loading of the broadcast_in_progress
     *             member field is also needed.
     */

    BOOl broadcast_in_progress = FALSE;
    LONG count = 0;
    EnterCriticalSection(&cond->access_waiting_threads_count_critsec);
    {
        count = --(cond->waiting_thread_count);
        
        /* Accessing this field inside the critical section to be sure to
         * get a synchronized value as set by broadcast.
         */
        broadcast_in_progress = cond->broadcast_in_progress;
    }
    LeaveCriticalSection(&cond->access_waiting_threads_count_critsec);
    
    BOOL all_waiting_threads_awake = TRUE;
    if (TRUE == broadcast_in_progress && count > 0) {
        all_waiting_threads_awake = FALSE;
    }
    
    if (TRUE == all_waiting_threads_awake) {
        /* Tell the signal or broadcast that all threads to wake up are awake
         * so the signal or broadcast can end and allow new waiters to add
         * themselves to the count.
         */
        BOOL const set_event_retval = SetEvent(cond->finished_waking_waiting_threads_event);
#if !defined(NDEBUG)
        assert(TRUE == set_event_retval);
        if (FALSE == set_event_retval) {
            /* This means that the signal or broadcast call will never return...
             * Their thread is lost, and if they were called from inside the
             * critical section with a locked mutex all threads will deadlock
             * while trying to lock the mutex.
             */
            
            /* Lock the mutex before handing back control to the caller.
             *
             * Current mutex implementation should assert in debug mode and not
             * return any error in non-debug mode.
             */
            int retval = amp_raw_mutex_lock(mutex);
            /* Error would surface earlier */
            assert(EINVAL != retval && "Mutex is invalid.");
            /* Error would surface earlier */
            assert(EDEADLK != retval && "Mutex is already locked by this thread."); 
            assert(AMP_SUCCESS == retval && "Unexpected error.");
            
            return EINVAL;
        }
#endif
    }
    
    
    /* Lock the mutex before handing back control to the caller.
     *
     * Current mutex implementation should assert in debug mode and not
     * return any error in non-debug mode.
     */
    int retval = amp_raw_mutex_lock(mutex);
    /* Error would surface earlier */
    assert(EINVAL != retval && "Mutex is invalid.");
    /* Error would surface earlier */
    assert(EDEADLK != retval && "Mutex is already locked by this thread."); 
    assert(AMP_SUCCESS == retval && "Unexpected error.");
    
    return AMP_SUCCESS;
}



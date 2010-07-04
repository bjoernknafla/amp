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
 * Shallow raw wrapper around the Windows CirticalSection primitive.
 *
 * @attention The debug mode doesn't find all errors and has some windows of
 *            opportunity for bugs to slip through - it is no 100% failure
 *            detection guarantee.
 *
 * TODO: @todo Is NDEBUG used by MSVC to indicate non-debug-mode or should I
 *             check another preprocessor symbol?
 */


#include "amp_mutex.h"

#include <assert.h>
#include <stddef.h>

#include "amp_stddef.h"
#include "amp_return_code.h"
#include "amp_raw_mutex.h"
#include "amp_internal_winthreads_critical_section_config.h"



int amp_raw_mutex_init(amp_mutex_t mutex)
{    
    BOOL retval = FALSE;

    assert(NULL != mutex);
    
    retval = InitializeCriticalSectionAndSpinCount(&mutex->critical_section,
        AMP_RAW_MUTEX_WINTHREADS_CRITICAL_SECTION_DEFAULT_SPIN_COUNT | AMP_RAW_MUTEX_WINTHREADS_CRITICAL_SECTION_CREATE_IMMEDIATELY_ON_WIN2000);
    
    if (FALSE == retval) {
        DWORD const last_error = GetLastError();
        return AMP_NOMEM;
    }
    
    /* Safe to set here as the mutex is only allowed to be used after init
     * returned. 
     */
    mutex->is_locked = FALSE;
    
    return AMP_SUCCESS;
}



int amp_raw_mutex_finalize(amp_mutex_t mutex)
{
#if !defined(NDEBUG)
    BOOL tryenterretval = FALSE;
#endif /* !defined(NDEBUG) */
    
    assert(NULL != mutex);
    
    /* Unexhaustive by-chance error checking in debug mode. */
#if !defined(NDEBUG)
    tryenterretval = TryEnterCriticalSection(&mutex->critical_section);
    if (TRUE == tryenterretval ) {
        
        BOOL const locked = mutex->is_locked;
        LeaveCriticalSection(&mutex->critical_section);
        
        if (TRUE == locked) {
            assert(0); /* Programming error */
            return AMP_ERROR;
        }    

    } else {
        assert(0); /* Programming error */
        return AMP_ERROR;
    }
#endif /* !defined(NDEBUG) */
    
    DeleteCriticalSection(&mutex->critical_section);
    
    return AMP_SUCCESS;
}



int amp_mutex_lock(amp_mutex_t mutex)
{
    assert(NULL != mutex);
    
    /* 
     * In init attributes are used to prevent lazy initialization on Win2000
     * and therefore no exceptions can be thrown because of lacking memory 
     * resources.
     *
     * An exception EXCEPTION_POSSIBLE_DEADLOCK might be thrown, however
     * MSDN advices not to handle to so the app crashes and can be debugged.
     *
     * See http://msdn.microsoft.com/en-us/library/ms682608(VS.85).aspx .
     */
    
    EnterCriticalSection(&mutex->critical_section);
    
    /* Check to prevent recursive locking on Windows in debug mode. */
#if !defined(NDEBUG)
    /* 
     * If possible to aquire lock while the critical section is locked
     * then this is a recursive locking and isn't allowed for
     * amp_raw_mutex.
     */
    if (TRUE == mutex->is_locked) {
        /*
         * Leave the critical section as amp_raw_mutex doesn't support recursive
         * locking in the first place.
         */
        LeaveCriticalSection(&mutex->critical_section);

        assert(0); /* Programming error - recursive locking is not allowed */
        
        return AMP_ERROR;
    }
#endif /* !defined(NDEBUG) */
    
    mutex->is_locked = TRUE;
    
    return AMP_SUCCESS;
}



int amp_mutex_trylock(amp_mutex_t mutex)
{
    int retval = AMP_SUCCESS;
    
    assert(NULL != mutex);    

    if (TryEnterCriticalSection(&mutex->critical_section)) {
        
#if !defined(NDEBUG)
        if (TRUE == mutex->is_locked) {
            LeaveCriticalSection(&mutex->critical_section);
            /* 
             * Trying to lock recursively leads to undefined behavior -
             * programming error.
             */
            assert(0);
            return AMP_ERROR;
        }
#endif
        
        mutex->is_locked = TRUE;
        
    } else {
        retval = AMP_BUSY;
    }
    
    return retval;
}



int amp_mutex_unlock(amp_mutex_t mutex)
{
    assert(NULL != mutex);

#if !defined(NDEBUG)
    /*
     * Add code to trylock the mutex, if not possible -> error.
     * If possible (Win allows recursive locking) and is_locked not 
     * set -> error, otherwise everything seems ok.
     */
    if (TryEnterCriticalSection(&mutex->critical_section)) {
        
        BOOL const locked = mutex->is_locked;
        LeaveCriticalSection(&mutex->critical_section);
        
        if (FALSE == locked) {
            /* 
             * Calling unlock for a non-locked mutex leads to undefined behavior -
             * programming error.
             */
            assert(0);
            return AMP_ERROR;
        }
        
        
    } else {
        /* 
         * Calling unlock for a mutex locked by another thread leads to 
         * undefined behavior - programming error.
         */
        assert(0);
        return AMP_ERROR;
    }
#endif    
    
    /* Minimal window of possible inconsistency.
     * Might allow recursive locking or trying to unlock an non-locked mutex, 
     * without any of the tests in debug mode detecting it - but that's why
     * these errors are documented to lead to undefined behavior.
     *
     * Why not set is_locked to FALSE after leaving the critical section?
     * Because only changes to vars inside the critical section are protected
     * by a memory fence and will be visible to other threads. Setting the
     * var outside of the critical section might not be visible to the debug
     * mode checks called by other threads and might therefore lead to heavily 
     * inconsistent and hard to debug and understand behavior...
     */
    mutex->is_locked = FALSE;
    LeaveCriticalSection(&mutex->critical_section);
    
    return AMP_SUCCESS;
}



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
 * Shallow wrapper around Windows semaphores.
 *
 * TODO: @todo Check for all Windows event waiting code if internal 
 *             event pumping events should be handled or not during waiting.
 *
 * TODO: @todo Check error handling with more infos about Windows error codes
 *
 * TODO: @todo Write tests cases to trigger errors like too many semaphores
 *             created, too many threads waiting, too many signals to understand
 *             the error codes of GetLastError.
 */



#include "amp_raw_semaphore.h"

/* Include NULL */
#include <stddef.h>

/* Include assert */
#include <assert.h>

/* Include errno, EINVAL, ENOSYS, ENOSPC, EPERM, EBUSY, EINTR, EDEADLK */
#include <errno.h>

/* TODO: @todo Remove include if not needed. */
/* #include <WinError.h> */



/* Include AMP_SUCCESS */
#include "amp_stddef.h"






int amp_raw_semaphore_init(struct amp_raw_semaphore_s *sem,
                           amp_semaphore_count_t init_count)
{
    assert(NULL != sem);
    
    if ((0ul > init_count) ||
        (AMP_RAW_SEMAPHORE_COUNT_MAX < init_count)) {
        
        /* TODO: @todo Decide if to assert or no to assert. */
        assert( (0ul <= init_count && AMP_RAW_SEMAPHORE_COUNT_MAX >= init_count) 
               && "init_count must be greater or equal to zero and lesser or equal to AMP_RAW_SEMAPHORE_COUNT_MAX.");
            
        return EINVAL;
    }
    
    int retval = AMP_SUCCESS;
    
    sem->semaphore_handle = CreateSemaphore(NULL, 
                                            (long)init_count, 
                                            (long)AMP_RAW_SEMAPHORE_COUNT_MAX,
                                            NULL);
    DWORD const last_error = GetLastError();
    if (NULL == sem->semaphore_handle) {
        
        switch (last_error) {
            case ERROR_TOO_MANY_SEMAPHORE:
                retval = EAGAIN;
                break;
            case ERROR_EXCL_SEM_ALREADY_OWNED:
                /* TODO: @todo Should this return EPERM? */
                assert(false 
                       && "It shouldn't happen that the (not) exclusive semaphore is owned by another process.");
                retval = EINVAL;
                break;
            /*case ERROR_SEM_IS_SET:
                break;*/
            /*case ERROR_TOO_MANY_SEM_REQUESTS:
                break;*/
            /*case ERROR_INVALID_AT_INTERRUPT_TIME:
                break;*/
            case ERROR_SEM_OWNER_DIED:
                /* TODO: @todo Check if this error can really happen. */
                assert(false 
                       && "It shouldn't happen that the previous ownership of the semaphore has ended.");
                retval = EINVAL;
                break;
            /*case ERROR_SEM_TIMEOUT:
                break;*/
            case ERROR_SEM_NOT_FOUND:
                assert(false && "It shouldn't happen that the (not) specified system semaphore name wasn't found.");
                retval = EINVAL;
                break;
            /*case ERROR_TOO_MANY_POSTS:
                break;*/
            default:
                /* TODO: @todo Check which code to use to flag an unknown error. 
                 */
                assert(false && "Unknown error.");
                retval = EINVAL;
                break;
        }
    } else {
        if (ERROR_ALREADY_EXISTS == last_error) {
            assert(ERROR_ALREADY_EXISTS != last_error 
                   && "Semaphore can't exist when created with name of NULL.");
            retval = EBUSY;;
        }
    }
    
    assert( (AMP_SUCCESS == retval 
             || EAGAIN == retval) 
           && "Unexpected error.");
    
    
    return retval;
}



int amp_raw_semaphore_finalize(struct amp_raw_semaphore_s *sem)
{
    assert(NULL != sem);
    
    int retval = AMP_SUCCESS;
    BOOL const close_retval = CloseHandle(sem->semaphore_handle);
    if (FALSE == close_retval) {
        DWORD const last_error = GetLastError();
        
        switch (last_error) {
            /*case ERROR_TOO_MANY_SEMAPHORE:
                retval = EAGAIN;
                break;*/
            /*case ERROR_EXCL_SEM_ALREADY_OWNED:
                assert(false 
                       && "It shouldn't happen that the (not) exclusive semaphore is owned by another process.");
                retval = EINVAL;
                break;*/
            case ERROR_SEM_IS_SET:
                /* TODO: @todo Need a Windows dev expert to check if this is the 
                 * right error code interpretation. 
                 */
                assert(false && "Mutex is in use.");
                
                retval = EBUSY;
                break;
                /*case ERROR_TOO_MANY_SEM_REQUESTS:
                 break;*/
                /*case ERROR_INVALID_AT_INTERRUPT_TIME:
                 break;*/
            case ERROR_SEM_OWNER_DIED:
                /* TODO: @todo Check if this error can really happen. */
                assert(false 
                       && "It shouldn't happen that the previous ownership of the semaphore has ended.");
                retval = EINVAL;
                break;
                /*case ERROR_SEM_TIMEOUT:
                 break;*/
            case ERROR_SEM_NOT_FOUND:
                assert(false && "It shouldn't happen that the (not) specified system semaphore name wasn't found.");
                retval = EINVAL;
                break;
                /*case ERROR_TOO_MANY_POSTS:
                 break;*/
            default:
                /* TODO: @todo Check which code to use to flag an unknown error. 
                 */
                assert(false && "Unknown error.");
                retval = EINVAL;
                break;
        }
    }
    
    assert(AMP_SUCCESS == retval && "Unexpected error.");
    
    return retval;
}



int amp_raw_semaphore_wait(struct amp_raw_semaphore_s *sem)
{
    assert(NULL != sem);
    
    int retval = AMP_SUCCESS;
    
    DWORD const wait_retval = WaitForSingleObject(sem->semaphore_handle,
                                                  INFINITE);
    if (WAIT_OBJECT_0 != wait_retval) {
        assert(WAIT_TIMEOUT != wait_retval 
               && "INFINITE wait interval shouldn't time out.");

        retval = EINVAL;
        
#if !defined(NDEBUG)
        DWORD const last_error = GetLastError();
        
        switch (last_error) {
                /*case ERROR_TOO_MANY_SEMAPHORE:
                 retval = EAGAIN;
                 break;*/
                /*case ERROR_EXCL_SEM_ALREADY_OWNED:
                 assert(false 
                 && "It shouldn't happen that the (not) exclusive semaphore is owned by another process.");
                 retval = EINVAL;
                 break;*/
            /* case ERROR_SEM_IS_SET:
                assert(false && "Mutex is in use.");
                
                retval = EBUSY;
                break; */
            case ERROR_TOO_MANY_SEM_REQUESTS:
                assert(false 
                       && "Unclear what is meant by requests - overflow in counter for waiting semaphores?");
                retval = EOVERFLOW;
                break;
                /*case ERROR_INVALID_AT_INTERRUPT_TIME:
                 break;*/
            case ERROR_SEM_OWNER_DIED:
                /* TODO: @todo Check if this error can really happen. */
                assert(false 
                       && "It shouldn't happen that the previous ownership of the semaphore has ended.");
                retval = EINVAL;
                break;
            case ERROR_SEM_TIMEOUT:
                assert(false && "INFINITE wait interval shouldn't time out.");
                retval = EINVAL;
                break;
            case ERROR_SEM_NOT_FOUND:
                assert(false && "It shouldn't happen that the (not) specified system semaphore name wasn't found.");
                retval = EINVAL;
                break;
            case ERROR_TOO_MANY_POSTS:
                assert(false && "If POSTS mean signals, then the max semaphore count has been reached.");
                retval = EOVERFLOW;
                break;
            default:
                /* TODO: @todo Check which code to use to flag an unknown error. 
                 */
                assert(false && "Unknown error.");
                retval = EINVAL;
                break;
        }
#endif /* !defined(NDEBUG) */
    }

    assert(AMP_SUCCESS == retval && "Unexpected error.");
    
    return retval;
}


int amp_raw_semaphore_signal(struct amp_raw_semaphore_s *sem)
{
    assert(NULL != sem);
    
    int retval = AMP_SUCCESS;
    BOOL const release_retval = ReleaseSemaphore(sem->semaphore_handle, 1, NULL);
    
    if (FALSE == release_retval) {
        DWORD const last_error = GetLastError();
        
        retval = EINVAL;
#if !defined(NDEBUG)
        DWORD const last_error = GetLastError();
        
        switch (last_error) {
                /*case ERROR_TOO_MANY_SEMAPHORE:
                 retval = EAGAIN;
                 break;*/
                /*case ERROR_EXCL_SEM_ALREADY_OWNED:
                 assert(false 
                 && "It shouldn't happen that the (not) exclusive semaphore is owned by another process.");
                 retval = EINVAL;
                 break;*/
            /*case ERROR_SEM_IS_SET:
                assert(false && "Mutex is in use.");
                
                retval = EBUSY;
                break;*/
            case ERROR_TOO_MANY_SEM_REQUESTS:
                assert(false 
                       && "Unclear what is meant by requests - overflow in counter for waiting semaphores?");
                retval = EOVERFLOW;
                break;
                /*case ERROR_INVALID_AT_INTERRUPT_TIME:
                 break;*/
            case ERROR_SEM_OWNER_DIED:
                /* TODO: @todo Check if this error can really happen. */
                assert(false 
                       && "It shouldn't happen that the previous ownership of the semaphore has ended.");
                retval = EINVAL;
                break;
            case ERROR_SEM_TIMEOUT:
                assert(false && "INFINITE wait interval shouldn't time out.");
                retval = EINVAL;
                break;
            case ERROR_SEM_NOT_FOUND:
                assert(false && "It shouldn't happen that the (not) specified system semaphore name wasn't found.");
                retval = EINVAL;
                break;
            case ERROR_TOO_MANY_POSTS:
                assert(false && "If POSTS mean signals, then the max semaphore count has been reached.");
                retval = EOVERFLOW;
                break;
            default:
                /* TODO: @todo Check which code to use to flag an unknown error. 
                 */
                assert(false && "Unknown error.");
                retval = EINVAL;
                break;
        }        
#endif /* !defined(NDEBUG) */
    }
    
    assert(AMP_SUCCESS == retval && "Unexpected error.");
    
    return retval;
}



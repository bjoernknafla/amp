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



#include "amp_semaphore.h"

#include <assert.h>
#include <stddef.h>

#include "amp_stddef.h"
#include "amp_return_code.h"
#include "amp_raw_semaphore.h"





int amp_raw_semaphore_init(amp_semaphore_t semaphore,
                           amp_semaphore_counter_t init_count)
{
    int retval = AMP_SUCCESS;
    DWORD last_error = 0;
    
    assert(NULL != semaphore);
    assert((amp_semaphore_counter_t)0 <= init_count);
    assert(AMP_RAW_SEMAPHORE_COUNT_MAX >= (amp_raw_semaphore_counter_t)init_count);
    
    if ((amp_semaphore_counter_t)0 > init_count
        || AMP_RAW_SEMAPHORE_COUNT_MAX < (amp_raw_semaphore_counter_t)init_count) {
        
        return AMP_ERROR;
    }

    semaphore->semaphore_handle = CreateSemaphore(NULL, 
                                                  (long)init_count, 
                                                  (long)AMP_RAW_SEMAPHORE_COUNT_MAX,
                                                  NULL);
    last_error = GetLastError();
    if (NULL == semaphore->semaphore_handle) {
        /* TODO: @todo Find out how to detect if not enough memory was the
         *             creation problem and return AMP_NOMEM accordingly.
         */
        return AMP_ERROR;
    } else {
        if (ERROR_ALREADY_EXISTS == last_error) {
            /* Semaphore can not already exist when created with name of NULL 
             * TODO: @todo Talk to a Windows expert if this is something that
             *              could happen otherwise only assert on it, do not use
             *              an if-then.
             */
            assert(0);
            retval = AMP_ERROR;
        }
    }
    
    return retval;
}



int amp_raw_semaphore_finalize(amp_semaphore_t semaphore)
{
    int retval = AMP_SUCCESS;
    
    assert(NULL != semaphore);
    
    if ( !CloseHandle(semaphore->semaphore_handle)) {
        DWORD const last_error = GetLastError();
        /* TODO: @todo Talk to a Windows expert and code more detailed error
         *             detection and handling.
         */
        
        assert(0); /* Programming error */
        retval = AMP_ERROR;
    }
    
    return retval;
}



int amp_semaphore_wait(amp_semaphore_t semaphore)
{
    int retval = AMP_SUCCESS;
    
    assert(NULL != semaphore);

    if (WAIT_OBJECT_0 != WaitForSingleObject(semaphore->semaphore_handle,
                                             INFINITE)) {
        /* TODO: @todo Talk to a Windows expert and code more detailed error
         *             detection and handling.
         */
        
        DWORD const last_error = GetLastError();
        assert(0); /* Programming error */

        retval = AMP_ERROR;
    }
    
    return retval;
}


int amp_semaphore_signal(amp_semaphore_t semaphore)
{
    int retval = AMP_SUCCESS;
    
    assert(NULL != semaphore);
    
    if ( !ReleaseSemaphore(semaphore->semaphore_handle, 1, NULL)) {
        /* TODO: @todo Talk to a Windows expert and code more detailed error
         *             detection and handling.
         */
        
        DWORD const last_error = GetLastError();
        
        assert(0); /* Programming error */
        retval = AMP_ERROR;
    }
    
    return retval;
}



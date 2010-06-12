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
 * Shallow wrapper around Windows thread-local storage.
 *
 * See http://msdn.microsoft.com/en-us/library/ms686749(VS.85).aspx
 * See http://support.microsoft.com/kb/94804
 * See http://msdn.microsoft.com/en-us/library/ms686991(VS.85).aspx
 * See http://msdn.microsoft.com/en-us/library/ms686997(VS.85).aspx
 * See http://msdn.microsoft.com/en-us/library/ms686801(VS.85).aspx
 * See http://msdn.microsoft.com/en-us/library/ms686804(VS.85).aspx
 * See http://msdn.microsoft.com/en-us/library/ms686818(VS.85).aspx
 * See http://msdn.microsoft.com/en-us/library/ms686812(VS.85).aspx
 */



#include "amp_raw_thread_local_slot.h"


#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include "amp_stddef.h"


int amp_raw_thread_local_slot_init(amp_thread_local_slot_key_t key)
{
    assert(NULL != key);

    if (NULL == key) {
        return EINVAL;
    }
    
    DWORD const index = TlsAlloc();
    
    if (TLS_OUT_OF_INDEXES == index) {
        assert(TLS_OUT_OF_INDEXES != index && "Maximum slot count exceeded.");
        
        /* TODO: @todo Call GetLastError to get better error diagnsotics. */
        
        return EAGAIN;
    }

    key->tls_index = index;
    
    return AMP_SUCCESS;
}



int amp_raw_thread_local_slot_finalize(amp_thread_local_slot_key_t key)
{
    BOOL const free_retval = TlsFree(key->tls_index);
    
    if (FALSE == free_retval) {
        assert(TRUE == free_retval && "Unknown error.");
        
        /* TODO: @todo Call GetLastError to get better error diagnsotics. */
        
        return EINVAL;
    }
    
    return AMP_SUCCESS;
}



int amp_thread_local_slot_set_value(amp_thread_local_slot_key_t key,
                                    void *value)
{
    BOOL const set_retval = TlsSetValue(key->tls_index, value);
    
    if (FALSE == set_retval) {
        assert(TRUE == set_retval && "Unknown error.");
        
        /* TODO: @todo Call GetLastError to get better error diagnsotics. */
        
        return EINVAL;
    }
    
    return AMP_SUCCESS;
}



void* amp_raw_thread_local_slot_value(amp_thread_local_slot_key_t key)
{
    assert(NULL != key);
    assert(NULL != value);
    
    /**
     * TODO: @todo Add a debug status flag to check if a key has been 
     * initialized correctly.
     *
     * If 0 is returned GetLastError might return ERROR_SUCCESS to indicate that
     * everything is alright - Pthreads doesn't detect errors but might lead
     * to undefined behavior when using an invalid key.
     */
    void *retval =  TlsGetValue(key->tls_index);
    DWORD const last_error = GetLastError();
    assert((NULL != retval || ERROR_SUCCESS == last_error)
           && "Unknown error.");
    
    return retval;
}



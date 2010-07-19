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
 * Implementation of amp memory.
 */ 

#include "amp_memory.h"

#include <assert.h>
#include <stdlib.h>

#include "amp_return_code.h"


void* amp_default_allocator_context = NULL;



static struct amp_raw_allocator_s amp_internal_allocator_default = {
    amp_default_alloc,
    amp_default_calloc,
    amp_default_dealloc,
    NULL /* amp_default_allocator_context */
};



amp_allocator_t amp_default_allocator = &amp_internal_allocator_default;



void* amp_default_alloc(void *dummy_allocator_context, 
                        size_t bytes_to_allocate,
                        char const* filename,
                        int line)
{
    (void)dummy_allocator_context;
    (void)filename;
    (void)line;
    
    return malloc(bytes_to_allocate);
}



void* amp_default_calloc(void* dummy_allocator_context,
                       size_t elem_count,
                       size_t bytes_per_elem,
                       char const* filename,
                       int line)
{
    (void)dummy_allocator_context;
    (void)filename;
    (void)line;
    
    return calloc(elem_count, bytes_per_elem);
}



int amp_default_dealloc(void *dummy_allocator_context,
                        void *pointer,
                        char const* filename,
                        int line)
{
    (void)dummy_allocator_context;
    (void)filename;
    (void)line;
    
    free(pointer);
    
    return AMP_SUCCESS;
}



int amp_allocator_create(amp_allocator_t* target_allocator,
                         amp_allocator_t source_allocator,
                         void* target_allocator_context,
                         amp_alloc_func_t target_alloc_func,
                         amp_calloc_func_t target_calloc_func,
                         amp_dealloc_func_t target_dealloc_func)
{
    amp_allocator_t tmp_allocator = NULL;
    
    assert(NULL != target_allocator);
    assert(NULL != source_allocator);
    assert(NULL != target_alloc_func);
    assert(NULL != target_calloc_func);
    assert(NULL != target_dealloc_func);
    
    tmp_allocator = (amp_allocator_t)AMP_ALLOC(source_allocator, sizeof(*tmp_allocator));
    if (NULL == tmp_allocator) {
        return AMP_NOMEM;
    }
    
    tmp_allocator->alloc_func = target_alloc_func;
    tmp_allocator->calloc_func = target_calloc_func;
    tmp_allocator->dealloc_func = target_dealloc_func;
    tmp_allocator->allocator_context = target_allocator_context;
    
    *target_allocator = tmp_allocator;
    
    return AMP_SUCCESS;
}



int amp_allocator_destroy(amp_allocator_t* target_allocator,
                          amp_allocator_t source_allocator)
{
    int return_code = AMP_ERROR;
    
    assert(NULL != target_allocator);
    assert(NULL != source_allocator);
    
    return_code = AMP_DEALLOC(source_allocator,
                              *target_allocator);
    if (AMP_SUCCESS == return_code) {
        
        *target_allocator = NULL;
        
    } else {
        assert(0); /* Programming error */
        return_code = AMP_ERROR;
    }
    
    return return_code;
}



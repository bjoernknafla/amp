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
 * Function typedefs for a memory allocator concept and shallow wrappers
 * to adapt the memory interface to C standard malloc and free.
 *
 * Amp's allocator and deallocator functions get an allocator context as their
 * first parameter to enable the library user to hand functions to amp that
 * work on specific memory pools passed to them via the allocator context.
 *
 * TODO: @todo Add allocators and deallocators that return 16 byte aligned
 *             memory and eventually add allocator interfaces to explicitly
 *             set the memory alignment for allocation and deallocation.
 *
 * TODO: @todo Add a unit test.
 *
 * TODO: @todo Decide if to add a calloc alike typedef and wrapper function
 *             based on own use and amp user feedback.
 */

#ifndef AMP_amp_memory_H
#define AMP_amp_memory_H



#include <stddef.h>



#if defined(__cplusplus)
extern "C" {
#endif
    

    
    
    
    typedef void* (*amp_alloc_func_t)(void *allocator_context, size_t bytes_to_allocate);
    
    /**
     * 
     *
     * Only call to free  memory allocated via the associated alloc function and
     * allocator context.
     */
    typedef void (*amp_dealloc_func_t)(void *allocator_context, void *pointer);
    
    
    
    /**
     * Shallow wrapper around C std malloc which ignores allocator context.
     *
     * Only thread-safe if C's std malloc is thread-safe.
     */
    void* amp_malloc(void *dummy_allocator_context, size_t bytes_to_allocate);
    
    
    
    /**
     * Shallow wrapper around C std malloc which ignores allocator context.
     *
     * Only thread-safe if C's std free is thread-safe.
     */
    void amp_free(void *dummy_allocator_context, void *pointer);
    
    
#if defined(__cplusplus)   
} /* extern "C" */
#endif


#endif /* AMP_amp_memory_H */

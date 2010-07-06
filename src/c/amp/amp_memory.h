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
 * An allocator used by create and destroy functions which can be 
 * configured by users to include their own allocation, array allocation, 
 * and deallocation functions using a user supplied context.
 *
 * The default allocator uses shallow wrappers around C's malloc, calloc, and
 * free.
 *
 * TODO: @todo Add a test, especially to check the amp_calloc_func_t contract.
 */

#ifndef AMP_amp_memory_H
#define AMP_amp_memory_H



#include <stddef.h>



#if defined(__cplusplus)
extern "C" {
#endif
    
    
    /**
     * Function type defining an allocation function that allocates memory
     * using the allocator_context and returns a pointer to the newly allocated
     * memory of size bytes_to_allocate or NULL if an error occured, e.g. if not
     * enough memory is available in the allocator_context to service the 
     * allocation request.
     */
    typedef void* (*amp_alloc_func_t)(void* allocator_context, 
                                      size_t bytes_to_allocate,
                                      char const* filename,
                                      int line);
    
    /**
     * Function type defining an allocation function that allocates
     * a contiguous chunk of memory that can hold elem_count times 
     * bytes_per_elem, sets the whole memory area to zero, and returns a pointer 
     * to the first element or NULL if an error occured, e.g. if not enough
     * memory is available in the allocator_context to service the allocation
     * request.
     */
    typedef void* (*amp_calloc_func_t)(void* allocator_context,
                                       size_t elem_count,
                                       size_t bytes_per_elem,
                                       char const* filename,
                                       int line);
                                       
    /**
     * Function type defining a deallocation function that frees the memory
     * pointed to by pointer that belongs to the allocator_context.
     *
     * Only call to free  memory allocated via the associated alloc function and
     * allocator context otherwise behavior might be undefined.
     *
     * Returns AMP_SUCCESS on successfull deallocation or function specific
     * error codes on errors.
     */
    typedef int (*amp_dealloc_func_t)(void* allocator_context, 
                                      void *pointer,
                                      char const* filename,
                                      int line);

    
    
    
    /**
     * Default allocator context for amp_default_alloc, amp_default_calloc, and 
     * amp_default_dealloc.
     *
     * @attention Do not change amp_default_allocator_context.
     */
    extern void* amp_default_allocator_context;
    
#define AMP_DEFAULT_ALLOCATOR_CONTEXT (amp_default_allocator_context)
    
    
    /**
     * Shallow wrapper around C std malloc which ignores allocator context,
     * filename, and line.
     *
     * Only thread-safe if C's std malloc is thread-safe.
     */
    void* amp_default_alloc(void *dummy_allocator_context, 
                            size_t bytes_to_allocate,
                            char const* filename,
                            int line);
    
    
    /**
     * Shallow wrapper around C std calloc which ignores the allocator context,
     * filename, and line.
     *
     * Only thread-safe if C's std calloc is thread-safe.
     */
    void* amp_default_calloc(void* dummy_allocator_context,
                             size_t elem_count,
                             size_t bytes_per_elem,
                             char const* filename,
                             int line);
    
    
    /**
     * Shallow wrapper around C std free which ignores allocator context,
     * filename, and line.
     *
     * Only thread-safe if C's std free is thread-safe.
     *
     * Always returns AMP_SUCCESS.
     */
    int amp_default_dealloc(void *dummy_allocator_context, 
                            void *pointer,
                            char const* filename,
                            int line);
    
    
    /**
     * Allocator type used by amp's create and destroy functions.
     * Treat as opaque as its implementation can and will change with each 
     * amp release or new version.
     *
     * Create via amp_allocator_create and destroy via amp_allocator_destroy.
     * To allocate or deallocate memory via an allocator use the functions
     * (which might be preprocessor macros) AMP_ALLOC, AMP_CALLOC, and 
     * AMP_DEALLOC.
     */
    struct amp_raw_allocator_s {
        amp_alloc_func_t alloc_func;
        amp_calloc_func_t calloc_func;
        amp_dealloc_func_t dealloc_func;
        void* allocator_context;
    };
    typedef struct amp_raw_allocator_s* amp_allocator_t;
    
    
#define AMP_ALLOCATOR_UNINITIALIZED ((amp_allocator_t)NULL)
    
    /**
     * Creates and initializes a target allocator using source_allocator to 
     * allocate memory for it.
     *
     * allocator_context can be NULL if alloc_func, calloc_func, and 
     * dealloc_func work with a NULL arguments passed to them for their 
     * allocator context.
     *
     * alloc_func, calloc_func, and dealloc_func and allocator_context must
     * work/fit together.
     *
     * @return AMP_SUCCESS on successful creation.
     *         AMP_NOMEM if not enough memory is available to allocate the
     *         target allocator.
     *         AMP_ERROR might be returned if an error is detected.
     */
    int amp_allocator_create(amp_allocator_t* target_allocator,
                             amp_allocator_t source_allocator,
                             void* allocator_context,
                             amp_alloc_func_t alloc_func,
                             amp_calloc_func_t calloc_func,
                             amp_dealloc_func_t dealloc_func);
    
    
    /**
     * Destroys the target allocator using source_allocator to deallocate its
     * memory.
     *
     * @return AMP_SUCCESS on successful creation.
     *         AMP_ERROR might be returned if an error is detected. Expect it
     *         but do not rely on it. AMP_ERROR might be returned if the
     *         dealloc func stored in target_allocator is not capable of
     *         deallocating the memory allocated via the target allocators
     *         alloc or calloc function.
     */
    int amp_allocator_destroy(amp_allocator_t* target_allocator,
                              amp_allocator_t source_allocator);
    

    
    
    
    /**
     * Default allocator using amp_default_alloc, amp_default_calloc,
     * amp_default_dealloc, and AMP_DEFAULT_ALLOCATOR_CONTEXT. Use it to 
     * bootstrap new allocators.
     *
     * @attention Do not change it.
     */
    extern amp_allocator_t amp_default_allocator;
    
    
    /**
     * Default allocator.
     */
#define AMP_DEFAULT_ALLOCATOR amp_default_allocator
    
    
    /**
     * Calls the alloc function of allocator to allocate size bytes of memory.
     * See amp_alloc_func_t for a behavior specification.
     *
     * The allocator expression must not have side-effects as it is used twice
     * in the macro.
     */
#define AMP_ALLOC(allocator, size) (allocator)->alloc_func((allocator)->allocator_context, (size), __FILE__, __LINE__)
    
    /**
     * Calls the calloc function of allocator to allocate elem_count times
     * elem_size bytes of memory.
     * See amp_calloc_func_t for a behavior specification.
     *
     * The allocator expression must not have side-effects as it is used twice
     * in the macro.
     */
#define AMP_CALLOC(allocator, elem_count, elem_size) (allocator)->calloc_func((allocator)->allocator_context, (elem_count), (elem_size), __FILE__, __LINE__)
    
    /**
     * Calls the dealloc function of allocator to deallocate the memory pointer
     * points to.
     * See amp_dealloc_func_t for a behavior specification.
     *
     * The allocator expression must not have side-effects as it is used twice
     * in the macro.
     */
#define AMP_DEALLOC(allocator, pointer) (allocator)->dealloc_func((allocator)->allocator_context, (pointer), __FILE__, __LINE__)
    
    
    
#if defined(__cplusplus)   
} /* extern "C" */
#endif


#endif /* AMP_amp_memory_H */

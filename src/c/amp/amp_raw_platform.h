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
 * Definition of struct amp_raw_platform_s to enable storing it on the stack.
 */

#ifndef AMP_amp_raw_platform_H
#define AMP_amp_raw_platform_H


#include <amp/amp_platform.h>



#if defined(__cplusplus)
extern "C" {
#endif

    
    
    /**
     * Context for amp_platform query functions. Treat as opaque and don't
     * rely on its implementation as it can change from version to version.
     *
     * TODO: @todo If WinVist and Win7 platform query functions always use the
     *             same buffer size for queries then change the implementation
     *             to use a buffer instead of alloc and dealloc all the time.
     */
    struct amp_raw_platform_s {
        void* allocator_context;
        amp_alloc_func_t alloc_func;
        amp_dealloc_func_t dealloc_func;
    };
    
    
    /**
     * Like amp_platform_create but does not allocate memory for the amp 
     * platform description structure itself but might allocate a query buffer.
     */
    int amp_raw_platform_init(amp_platform_t descr,
                              amp_allocator_t allocator);
    
    /**
     * Like amp_platform_destroy but only finalizes descr and frees the 
     * memory of its members but doesn't deallocate the platform structure 
     * itself.
     */
    int amp_raw_platform_finalize(amp_platform_t descr,
                                  amp_allocator_t allocator);
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif
        

#endif /* AMP_amp_raw_platform_H */

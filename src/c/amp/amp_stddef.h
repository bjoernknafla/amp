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
 * Base type and preprocessor symbol definitions.
 */


#ifndef AMP_amp_stddef_H
#define AMP_amp_stddef_H


#include <stddef.h>



#if defined(__cplusplus)
extern "C" {
#endif
    

    
    
#if !defined(AMP_BOOL)
#   define AMP_BOOL int
#endif

#define AMP_TRUE ((AMP_BOOL)1)
#define AMP_FALSE ((AMP_BOOL)0)

    
    
#define AMP_SIZE_MAX (~((size_t)0))
    

    
#if !defined(AMP_BYTE)
/**
 * Byte sized unsigned integral type.
 *
 * The C standard doesn't define that a char must have 8 bits. 
 * If this assumption doesn't hold true define AMP_BYTE to the platforms byte
 * sized type and amp will adapt your definition.
 */
#   define AMP_BYTE unsigned char
#endif


typedef AMP_BOOL amp_bool_t;
typedef AMP_BYTE amp_byte_t;
    
    
#if defined(__cplusplus)
} /* extern"C" */
#endif

    
#endif /* #ifndef AMP_amp_stddef_H */


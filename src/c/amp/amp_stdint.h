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
 * Minimal cross-platform subset of types of C99 stdint needed by amp.
 *
 * Assumes a C99 compatible C compiler, or MSVC, or a compiler whose c lib
 * contains the stdint header.
 *
 * Makes intptr_t,  and uintptr_t accessible to amp.
 *
 * If C89 needs to be supported or a platform needs a better platform detection
 * and handling then poc ( http://github.com/bjoernknafla/poc ) can be used to 
 * define the necessary bit-sized primitive integer types. Currently poc isn't
 * used to keep the dependencies of amp at a minimum.
 *
 * amp uses intptr_t and uintptr_t in places where intmax_t and uintmax_t would
 * be more appropriate. However making intmax_t and uintmax_t available in a 
 * cross platform way complicates the implementation and adds more complexity
 * than it seems worth just yet.
 */ 

#ifndef AMP_amp_stdint_H
#define AMP_amp_stdint_H


#if defined(_MSC_VER)
#   include <stddef.h> /* MSVC defines intptr_t, uintptr_t in stddef.h */
#elif defined(__GNUC__)
#   include <stdint.h> /* C99 header with intptr_t, uintptr_t */
#elif defined(__llvm__) && defined(__clang__)
#   include <stdint.h> /* C99 header with intptr_t, uintptr_t */
#else
#   error Unsupported platform.
#endif


#endif /* AMP_amp_stdint_H */

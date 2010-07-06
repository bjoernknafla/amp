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
 * sysconf based platform detection - supported by Linux, BSD, OS X.
 *
 * amp_platform_create and amp_platform_destroy are implemented in 
 * amp_platform_common.c.
 *
 * TODO: @todo Check that the number of supported hardware threads is returned
 *             and not only the number of full cores or even only the number
 *             of processor dies.
 *
 * See http://software.intel.com/en-us/articles/utilizing-processor-performance-in-rich-internet-applications/
 * 
 * See http://developer.apple.com/mac/library/documentation/Darwin/Reference/ManPages/man3/sysconf.3.html#//apple_ref/doc/man/3/sysconf
 *
 * See http://sources.redhat.com/ml/glibc-linux/1999-q3/msg00036.html
 *
 * See http://compute.cnr.berkeley.edu/cgi-bin/man-cgi?sysconf+3
 *
 */

#include "amp_platform.h"

#include <assert.h>
#include <stddef.h>

#include <unistd.h>

#include "amp_return_code.h"



static size_t amp_internal_platform_get_core_count(void);
static size_t amp_internal_platform_get_core_count(void)
{
    long result = sysconf(_SC_NPROCESSORS_CONF);
    
    if (-1l == result) {
        result = 0l;
    }
    
    return (size_t)result;
}



static size_t amp_internal_platform_get_active_core_count(void);
static size_t amp_internal_platform_get_active_core_count(void)
{
    /* TODO: @todo Check on Linux if this really works - does not work on 
     *             Mac OS X 10.6.2, same result for CONF and ONLN. */
    
    long result = sysconf(_SC_NPROCESSORS_ONLN);
    
    if (-1l == result) {
        result = 0l;
    }
    
    return (size_t)result;
}



int amp_platform_get_installed_core_count(amp_platform_t descr, 
                                          size_t* result)
{
    (void)descr;
    
    assert(NULL != descr);
    
    if (NULL != result) {
        
        *result = amp_internal_platform_get_core_count();
    }
    
    return AMP_SUCCESS;
}



int amp_platform_get_active_core_count(amp_platform_t descr, 
                                       size_t* result)
{
    (void)descr;
    
    assert(NULL != descr);
    
    if (NULL != result ) {
        *result = amp_internal_platform_get_active_core_count();
    }
    
    return AMP_SUCCESS;
}



int amp_platform_get_installed_hwthread_count(amp_platform_t descr, 
                                              size_t* result)
{
    (void)descr;
    
    assert(NULL != descr);
    
    /* Functionality not supported, no value returned. */
    (void)result;
    
    return AMP_UNSUPPORTED;
}



int amp_platform_get_active_hwthread_count(amp_platform_t descr, 
                                               size_t* result)
{
    (void)descr;
    
    assert(NULL != descr);
    
    /* Functionality not supported, no value returned. */
    (void)result;
    
    return AMP_UNSUPPORTED;
}




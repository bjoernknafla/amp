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
 * Platform detection functions to query the concurrency level of the 
 * hardware and operating system combination. Assumes that the hardware
 * is homogeneous and only queries the architecture the software runs on, i.e.
 * doesn't detect the GPU when run on CPU.
 *
 * TODO: @todo Add detection for the number of packages (CPU dies), cores per
 *             package, hw-threads per core. Take non-homoheneous platforms into
 *             account. Also detect memory and cache sizes and cache / local
 *             storage grouping of hw-threads and cores and detect if the 
 *             platform is an UMA or NUMA architecture. Add ways to detect if
 *             specific infos can be determined or not on the platform in use.
 * TODO: @todo Decide if to implement the following functions:
 *             size_t amp_platform_get_package_type_count(void);
 *             size_t amp_platform_get_package_count_for_type(size_t package_type_id);
 *             size_t amp_platform_get_core_type_count(size_t package_type_id);
 *             size_t amp_platform_get_installed_core_count_for_type(size_t package_type_id, 
 *                                                         size_t core_type_for_package_type_id);
 *             size_t amp_platform_get_installed_hwthread_count(size_t package_type_id, 
 *                                                    size_t core_type_for_package_type_id);
 * TODO: @todo Add source file using GetLogicalProcessorInformationEx to query
 *             the platform on Windows Vista or later Windows versions.
 *
 */


#ifndef AMP_amp_platform_H
#define AMP_amp_platform_H


#include <stddef.h>

#include <amp/amp_memory.h>



#if defined(__cplusplus)
extern "C" {
#endif
        
    
#define AMP_PLATFORM_UNINITIALIZED NULL
    
    /**
     * Opaque data type to enable querying platform information.
     */
    typedef struct amp_raw_platform_s* amp_platform_t;


    
    
    
    /**
     * Creates a platform context to enable allocation and deallocation of
     * memory if needed by the platform query functions (necessary for Windows 
     * Vista and Windows7 platform detection).
     *
     * The allocator (allocator_context, alloc_func, and dealloc_func) are used
     * to allocate the memory for descr and eventually to create a temporary
     * buffer needed to query the platforms hardware informations (Windows
     * GetLogicalProcessorInformation needs a dynamic buffer).
     *
     * On error no memory is leaked.
     *
     * Call amp_platform_destroy to finalize the platform description.
     *
     * @return AMP_SUCCESS on successful creation of a platform description.
     *         EINVAL is descr, alloc_func, or dealloc_func are NULL.
     */
    int amp_platform_create(amp_platform_t* descr,
                            void* allocator_context,
                            amp_alloc_func_t alloc_func,
                            amp_dealloc_func_t dealloc_func);
    
    /**
     * Finalizes descr (invalidates it), frees its memory.
     *
     * Returns AMP_SUCCESS on successful destruction of descr, otherwise
     * another error code is returned and if allocator_context, and
     * dealloc_func aren't valid to deallocate descr behavior is undefined and
     * descr will very likely be in an unusable and non-rescueable state.
     */
    int amp_platform_destroy(amp_platform_t* descr,
                             void* allocator_context,
                             amp_dealloc_func_t dealloc_func);
    
    
    
    /**
     * Queries the platform for the number of processor cores (active or not).
     *
     * descr must not be NULL.
     *
     * If the platform can be queried for this information the result is 
     * stored at the memory location result points to and AMP_SUCCESS is 
     * returned by the function. If result is NULL the function return value
     * (see below) indicates if the information can be queried.
     *
     * If the information can not be queried descr isn't touched or changed.
     * 
     * @return AMP_SUCCESS if the information can be queried or
     *         ENOSYS if the information can not be queried.
     *         EINVAL is returned if desc is invalid, e.g. NULL.
     */
    int amp_platform_get_installed_core_count(amp_platform_t descr, 
                                              size_t* result);

    /**
     * Queries the platform for the number of currently active processor cores.
     *
     * descr must not be NULL.
     *
     * If the platform can be queried for this information the result is 
     * stored at the memory location result points to and AMP_SUCCESS is 
     * returned by the function. If result is NULL the function return value
     * (see below) indicates if the information can be queried.
     *
     * If the information can not be queried descr isn't touched or changed.
     * 
     * @return AMP_SUCCESS if the information can be queried or
     *         ENOSYS if the information can not be queried.
     *         EINVAL is returned if desc is invalid, e.g. NULL.
     */
    int amp_platform_get_active_core_count(amp_platform_t descr, 
                                           size_t* result);
    
    /**
     * Queries the platform for the number of hardware threads - if the platform
     * support simultaneous multithreading (and supports querying this 
     * information) then the number returned might be greater than the one
     * returned by amp_platform_get_installed_core_count.
     *
     * descr must not be NULL.
     *
     * If the platform can be queried for this information the result is 
     * stored at the memory location result points to and AMP_SUCCESS is 
     * returned by the function. If result is NULL the function return value
     * (see below) indicates if the information can be queried.
     *
     * If the information can not be queried descr isn't touched or changed.
     * 
     * @return AMP_SUCCESS if the information can be queried or
     *         ENOSYS if the information can not be queried.
     *         EINVAL is returned if desc is invalid, e.g. NULL.
     */
    int amp_platform_get_installed_hwthread_count(amp_platform_t descr, 
                                                  size_t* result);
    
    /**
     * Queries the platform for the number of currently active hardware threads 
     * - if the platform support simultaneous multithreading (and supports 
     * querying this information) then the number returned might be greater than 
     * the one returned by amp_platform_get_active_core_count.
     *
     * descr must not be NULL.
     *
     * If the platform can be queried for this information the result is 
     * stored at the memory location result points to and AMP_SUCCESS is 
     * returned by the function. If result is NULL the function return value
     * (see below) indicates if the information can be queried.
     *
     * If the information can not be queried descr isn't touched or changed.
     * 
     * @return AMP_SUCCESS if the information can be queried or
     *         ENOSYS if the information can not be queried.
     *         EINVAL is returned if desc is invalid, e.g. NULL.
     */
    int amp_platform_get_active_hwthread_count(amp_platform_t descr, 
                                               size_t* result);
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif


#endif /* AMP_amp_platform_H */

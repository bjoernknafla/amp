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
 * Unit tests for launching and joining with amp_thread and therefore indirectly
 * with amp_raw_thread.
 */


#include <UnitTest++.h>

// Include std::size_t
#include <cstddef>

// Include AMP_SUCCESS
#include <amp/amp_stddef.h>
#include <amp/amp_return_code.h>
#include <amp/amp_memory.h>
#include <amp/amp_thread.h>



namespace
{
    int const launch_run_join_success_value = 42;
    
    void launch_run_join_thread_func(void *context)
    {
        int* value_to_set = static_cast<int*>(context);
        *value_to_set = launch_run_join_success_value;
    }
    
} // anonymous namespace


SUITE(amp_thread)
{
    TEST(launch_run_join)
    {
        std::size_t const thread_count = 64;
        
        amp_thread_t threads[thread_count];
        int values_to_set[thread_count] = {0};
        
        for (std::size_t i = 0; i < thread_count; ++i) {
            int retval = amp_thread_create_and_launch(&threads[i],
                                                      AMP_DEFAULT_ALLOCATOR,
                                                      &values_to_set[i], 
                                                      &launch_run_join_thread_func);
            CHECK_EQUAL(AMP_SUCCESS, retval);
        }
        
        for (std::size_t i = 0; i < thread_count; ++i) {
            int retval = amp_thread_join_and_destroy(&threads[i],
                                                     AMP_DEFAULT_ALLOCATOR);
            CHECK_EQUAL(AMP_SUCCESS, retval);
        }
        
        for (std::size_t i = 0; i < thread_count; ++i) {
            CHECK_EQUAL(launch_run_join_success_value, values_to_set[i]);
        }
        
    }
    
}


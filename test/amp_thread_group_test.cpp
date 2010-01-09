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

#include <UnitTest++.h>

#include <vector>

#include <stddef.h>
#include <assert.h>
#include <errno.h>

#include <amp/amp_stddef.h>
#include <amp/amp_thread_group.h>
#include <amp/amp_memory.h>
#include <amp/amp_raw_byte_range.h>



SUITE(amp_thread_group)
{
    namespace {
        
        int const fortytwo = 42;
        
        void set_int_context_to_fortytwo(void *ctxt)
        {
            int *item = (int *)ctxt;
            *item = 42;
        }
        
        
        struct thread_context_s {
          
            int *item_to_set;
            
            int item_value_to_use_for_set;
        };
        
        
        void set_context_defined_value(void *ctxt)
        {
            struct thread_context_s *context = (struct thread_context_s *)ctxt;
            
            int const item_value_to_use = context->item_value_to_use_for_set;
            
            *(context->item_to_set) = item_value_to_use;
        }
        
        
        void set_context_defined_value_multiplied_by_fortytwo(void *ctxt)
        {
            struct thread_context_s *context = (struct thread_context_s *)ctxt;
            
            int const item_value_to_use = context->item_value_to_use_for_set;
            
            *(context->item_to_set) = item_value_to_use * fortytwo;
        }
    
    } // anonymous namespace
    
    
    TEST(create_launch_join_destroy)
    {
        
        amp_thread_group_t thread_group;
        
        struct amp_thread_group_context_s thread_group_context;
        thread_group_context.alloc_func = amp_malloc;
        thread_group_context.dealloc_func = amp_free;
        thread_group_context.allocator_context = NULL;
        
        size_t const thread_count = 16;
        std::vector<int> values_to_write_vector(thread_count, 0);
        
        typedef std::vector<thread_context_s> context_vector_type;
        context_vector_type context_vector(thread_count);
        for (size_t i = 0; i < thread_count; ++i) {
            context_vector[i].item_to_set = &values_to_write_vector[i];
            context_vector[i].item_value_to_use_for_set = i;
        }
        
        std::vector<amp_raw_thread_func_t> thread_function_vector;
        for (size_t i = 0; i < thread_count; ++i) {
            if (i < (thread_count / 2)) {
                thread_function_vector.push_back(set_context_defined_value);
            } else {
                thread_function_vector.push_back(set_context_defined_value_multiplied_by_fortytwo);
            }
            
        }
        
        
        struct amp_raw_byte_range_s context_range;
        int retval = amp_raw_byte_range_init_with_item_count(&context_range,
                                                             &context_vector[0], 
                                                             thread_count,
                                                             sizeof(context_vector_type::value_type));
        assert(AMP_SUCCESS == retval);
        
        struct amp_raw_byte_range_s function_range;
        retval = amp_raw_byte_range_init_with_item_count(&function_range,
                                                         &thread_function_vector[0],
                                                         thread_count,
                                                         sizeof(amp_raw_thread_func_t));
        assert(AMP_SUCCESS == retval);
        
        retval = amp_thread_group_create(&thread_group,
                                         &thread_group_context,
                                         thread_count,
                                         &context_range,
                                         amp_raw_byte_range_next,
                                         &function_range,
                                         amp_raw_byte_range_next_func);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        size_t number_of_joinable_threads = 0;
        retval = amp_thread_group_get_joinable_thread_count(thread_group, &number_of_joinable_threads);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        CHECK_EQUAL(static_cast<size_t>(0), number_of_joinable_threads);
        
        
        size_t number_of_joinable_threads_after_launch_all = 0;
        retval = amp_thread_group_launch_all(thread_group, &number_of_joinable_threads_after_launch_all);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        CHECK_EQUAL(thread_count, number_of_joinable_threads_after_launch_all);
        
        
        number_of_joinable_threads = 0;
        retval = amp_thread_group_get_joinable_thread_count(thread_group, &number_of_joinable_threads);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        CHECK_EQUAL(static_cast<size_t>(thread_count), number_of_joinable_threads);
        
        
        size_t number_of_joinable_threads_after_join_all = 0;
        retval = amp_thread_group_join_all(thread_group, &number_of_joinable_threads_after_join_all);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        CHECK_EQUAL(static_cast<size_t>(0), number_of_joinable_threads_after_join_all);
        
        
        number_of_joinable_threads = 0;
        retval = amp_thread_group_get_joinable_thread_count(thread_group, &number_of_joinable_threads);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        CHECK_EQUAL(static_cast<size_t>(0), number_of_joinable_threads);
        
        
        // struct amp_thread_group_context_s *ctxt = NULL;
        
        // retval = amp_thread_group_get_context(thread_group, &ctxt);
        // CHECK_EQUAL(AMP_SUCCESS, retval);
        // CHECK_EQUAL(ctxt, &thread_group_context);
        
        
        retval = amp_thread_group_destroy(thread_group,
                                          &thread_group_context);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        for (size_t i = 0; i < thread_count; ++i) {
            if (i < (thread_count / 2)) {
                CHECK_EQUAL(static_cast<int>(i), values_to_write_vector[i]); 
            } else {
                CHECK_EQUAL(static_cast<int>(i) * fortytwo, values_to_write_vector[i]); 
            }
        }
    }
    
    
    
    TEST(create_single_func_launch_join_destroy)
    {
        
        amp_thread_group_t thread_group;
        
        struct amp_thread_group_context_s thread_group_context;
        thread_group_context.alloc_func = amp_malloc;
        thread_group_context.dealloc_func = amp_free;
        thread_group_context.allocator_context = NULL;
        
        size_t const thread_count = 16;
        typedef std::vector<int> context_vector_type;
        context_vector_type context_vector(thread_count, 0);
    
        struct amp_raw_byte_range_s context_range;
        int retval = amp_raw_byte_range_init_with_item_count(&context_range,
                                                             &context_vector[0], 
                                                             thread_count,
                                                             sizeof(context_vector_type::value_type));
        assert(AMP_SUCCESS == retval);
        
        retval = amp_thread_group_create_with_single_func(&thread_group,
                                                          &thread_group_context,
                                                          thread_count,
                                                          &context_range,
                                                          amp_raw_byte_range_next,
                                                          set_int_context_to_fortytwo);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        size_t number_of_joinable_threads = 0;
        retval = amp_thread_group_get_joinable_thread_count(thread_group, &number_of_joinable_threads);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        CHECK_EQUAL(static_cast<size_t>(0), number_of_joinable_threads);
        
        
        size_t number_of_joinable_threads_after_launch_all = 0;
        retval = amp_thread_group_launch_all(thread_group, &number_of_joinable_threads_after_launch_all);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        CHECK_EQUAL(thread_count, number_of_joinable_threads_after_launch_all);
        
        
        number_of_joinable_threads = 0;
        retval = amp_thread_group_get_joinable_thread_count(thread_group, &number_of_joinable_threads);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        CHECK_EQUAL(static_cast<size_t>(thread_count), number_of_joinable_threads);
        
        
        size_t number_of_joinable_threads_after_join_all = 0;
        retval = amp_thread_group_join_all(thread_group, &number_of_joinable_threads_after_join_all);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        CHECK_EQUAL(static_cast<size_t>(0), number_of_joinable_threads_after_join_all);
        
        
        number_of_joinable_threads = 0;
        retval = amp_thread_group_get_joinable_thread_count(thread_group, &number_of_joinable_threads);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        CHECK_EQUAL(static_cast<size_t>(0), number_of_joinable_threads);
        
        
        // struct amp_thread_group_context_s *ctxt = NULL;
        
        // retval = amp_thread_group_get_context(thread_group, &ctxt);
        // CHECK_EQUAL(AMP_SUCCESS, retval);
        // CHECK_EQUAL(ctxt, &thread_group_context);
        
        
        retval = amp_thread_group_destroy(thread_group,
                                          &thread_group_context);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        for (size_t i = 0; i < thread_count; ++i) {
            CHECK_EQUAL(fortytwo, context_vector[i]); 
        }
        
    }
    
    
    
} // SUITE(amp_thread_group)



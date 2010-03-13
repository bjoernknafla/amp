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
 * Unit tests for amp_raw_thread_local_slot.
 */


#include <UnitTest++.h>



// Include assert
#include <cassert>



#include <amp/amp_raw_thread_local_slot.h>
#include <amp/amp_raw_thread.h>
#include <amp/amp_stddef.h>



SUITE(amp_raw_thread_local_slot)
{
    
    // Disable the error-triggering checks to be able to run all tests.
#if 0
    TEST(non_initialized_slot)
    {
        amp_raw_thread_local_slot_key_t key;
        
        int retval = amp_raw_thread_local_slot_finalize(key);
        CHECK(AMP_SUCCESS != retval);
        
        int value = 23;
        retval = amp_raw_thread_local_slot_set_value(key, &value);
        
        CHECK(NULL == amp_raw_thread_local_slot_get_value(key));
    }
    
    
    
    TEST(after_finalization)
    {
        amp_raw_thread_local_slot_key_t key;
        
        int retval = amp_raw_thread_local_slot_init(&key);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        // Finalize immediately and check that the slot is invalid for 
        // later use afterwards.
        retval = amp_raw_thread_local_slot_finalize(key);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        
        
        
        retval = amp_raw_thread_local_slot_finalize(key);
        CHECK(AMP_SUCCESS != retval);
        
        int value = 23;
        retval = amp_raw_thread_local_slot_set_value(key, &value);
        
        CHECK(NULL == amp_raw_thread_local_slot_get_value(key));
    }
#endif // 0    
    
    
    TEST(single_thread)
    {
        
        amp_raw_thread_local_slot_key_t key;
        
        int retval = amp_raw_thread_local_slot_init(&key);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK(NULL == amp_raw_thread_local_slot_get_value(key));
              
        int data0 = 42;
        
        retval = amp_raw_thread_local_slot_set_value(key, &data0);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        void *result_data = amp_raw_thread_local_slot_get_value(key);
        CHECK(result_data == (void*)(&data0));
        CHECK_EQUAL(*((int*)result_data), data0);
        
        retval = amp_raw_thread_local_slot_finalize(key);
        CHECK_EQUAL(AMP_SUCCESS, retval);
    }
    
    
    
    TEST(single_thread_multiple_sets_and_gets)
    {
        
        amp_raw_thread_local_slot_key_t key;
        
        int retval = amp_raw_thread_local_slot_init(&key);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK(NULL == amp_raw_thread_local_slot_get_value(key));
        
        int data0 = 42;
        
        retval = amp_raw_thread_local_slot_set_value(key, &data0);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        void *result_data = amp_raw_thread_local_slot_get_value(key);
        CHECK(result_data == (void*)(&data0));
        CHECK_EQUAL(*((int*)result_data), data0);
        
        
        
        int data1 = 9;
        retval = amp_raw_thread_local_slot_set_value(key, &data1);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        result_data = amp_raw_thread_local_slot_get_value(key);
        CHECK(result_data == (void*)(&data1));
        CHECK_EQUAL(*((int*)result_data), data1);
        
        
        
        retval = amp_raw_thread_local_slot_set_value(key, NULL);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        result_data = amp_raw_thread_local_slot_get_value(key);
        CHECK(result_data == NULL);
        
        
        retval = amp_raw_thread_local_slot_finalize(key);
        CHECK_EQUAL(AMP_SUCCESS, retval);
    }
    
    
    namespace
    {
        typedef int check_flag_t;
        
        check_flag_t const CHECK_FLAG_UNSET = 0;
        check_flag_t const CHECK_FLAG_SET = 512;
        
        
        struct tls_thread_func_context_s {
            amp_raw_thread_local_slot_key_t *key_p;
            int slot_value;
            check_flag_t check_flag;
        };
        
        void tls_thread_func(void *context) 
        {
            struct tls_thread_func_context_s *ctxt = static_cast<struct tls_thread_func_context_s *>(context);
            
            int retval = amp_raw_thread_local_slot_set_value(*(ctxt->key_p), 
                                                                       &(ctxt->slot_value));
            assert(AMP_SUCCESS == retval);
            if (AMP_SUCCESS != retval) {
                return;
            }
            
            void *slot_return_value = amp_raw_thread_local_slot_get_value(*(ctxt->key_p));
            
            assert(NULL != slot_return_value);
            if (NULL == slot_return_value) {
                return;
            }
            
            
            if (*(static_cast<int*>(slot_return_value)) == ctxt->slot_value) {
                ctxt->check_flag = CHECK_FLAG_SET;
            }
            
        }
        
    } // anonymous namespace
    
    
    TEST(init_slot_run_multiple_threads)
    {

        // Create thread local slot. Create threads. Let each thread 
        // write its own data into the slot. Check that each thread
        // finds its own stored data in the slot.
        amp_raw_thread_local_slot_key_t key;
        int retval = amp_raw_thread_local_slot_init(&key);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        // First the main thread sets its slot data.
        
        
        
        // Start threads, each setting and then getting and checking its data.
        size_t const thread_count = 20;
        struct amp_raw_thread_s threads[thread_count];
        
        struct tls_thread_func_context_s thread_context_values[thread_count];
        
        
        for (size_t i = 0; i < thread_count; ++i) {
            thread_context_values[i].key_p = &key;
            thread_context_values[i].slot_value = i;
            thread_context_values[i].check_flag = CHECK_FLAG_UNSET;
            
            int const ret = amp_raw_thread_launch(&threads[i], 
                                                  &thread_context_values[i],
                                                  tls_thread_func);
            CHECK_EQUAL(AMP_SUCCESS, ret);
        }
        
        
        for (size_t i = 0; i < thread_count; ++i) {
            int const ret = amp_raw_thread_join(&threads[i]);
            CHECK_EQUAL(AMP_SUCCESS, ret);
        }
        
        for (size_t i = 0; i < thread_count; ++i) {
            CHECK_EQUAL(CHECK_FLAG_SET, thread_context_values[i].check_flag);
        }
        
        retval = amp_raw_thread_local_slot_finalize(key);
        CHECK_EQUAL(AMP_SUCCESS, retval);
    }
    
    
    TEST(start_multiple_threads_then_create_slot)
    {
        // Create threads and let them wait on a semaphore.
        // Create slot and signal the semaphore to let all threads pass.
        // Each thread writes its own data into the slot and then checks that
        // the data is what it wrote.
        
    }
    
    
} // SUITE(amp_raw_thread_local_slot)


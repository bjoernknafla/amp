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
 * Unit test for byte ranges.
 */

#include <UnitTest++.h>

#include <stddef.h>

#include <amp/amp_raw_byte_range.h>
#include <amp/amp_stddef.h>




SUITE(amp_raw_byte_range)
{
    TEST(empty_range_with_item_count)
    {
        size_t const ia_size = 10;
        int ia[ia_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init_with_item_count(&range, &ia[0], 0, sizeof(ia[0]));
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range));
        
    }
    
    
    
    TEST(one_item_range_with_item_count)
    {
        size_t const ia_size = 10;
        int ia[ia_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init_with_item_count(&range, &ia[0], 1, sizeof(ia[0]));
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));
        
        CHECK_EQUAL(ia[0], *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range));
    }
    
    
    
    TEST(multiple_item_range_with_item_count)
    {
        size_t const ia_size = 10;
        int ia[ia_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init_with_item_count(&range, &ia[3], 3, sizeof(ia[0]));
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));
        
        CHECK_EQUAL(ia[3], *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));
        
        CHECK_EQUAL(ia[4], *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));
        
        CHECK_EQUAL(ia[5], *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range));
    }
    
    
    
    TEST(empty_range_with_end_gap)
    {
        size_t const ia_size = 10;
        int ia[ia_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, &ia[0], ((AMP_BYTE *)&ia[0]) + 1, sizeof(int), sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval); 
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range));
    }
    
    

    TEST(empty_range_without_end_gap)
    {
        size_t const ia_size = 10;
        int ia[ia_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, &ia[0], &ia[0], sizeof(int), sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval); 
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range));
    }
    
    
    
    TEST(one_item_range_with_end_gap)
    {
        size_t const ia_size = 10;
        int ia[ia_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, &ia[0], ((AMP_BYTE *)&ia[1]) + 1, sizeof(int), sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval); 
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(ia[0], *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range));  
    }
    
    
    
    TEST(one_item_range_without_end_gap)
    {
        size_t const ia_size = 10;
        int ia[ia_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, &ia[0], &ia[1], sizeof(int), sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval); 
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(ia[0], *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range));  
    }
    
    
    
    TEST(multi_item_range_with_end_gap)
    {
        size_t const ia_size = 10;
        int ia[ia_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, &ia[0], ((AMP_BYTE *)&ia[3]) + 1, sizeof(int), sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval); 
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(ia[0], *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(ia[1], *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(ia[2], *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range));  
    }
    
    
    
    TEST(multi_item_range_without_end_gap)
    {
        size_t const ia_size = 10;
        int ia[ia_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, &ia[0], &ia[3], sizeof(int), sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval); 
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(ia[0], *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(ia[1], *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(ia[2], *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range));          
    }
    
    
    namespace {
        
        struct non_uniform {
            int i;
            double d;
        };
        
    } // anonymous namespace
    
    
    
    TEST(multi_item_init_interleaved_with_end_gap)
    {
        size_t const array_size = 10;
        struct non_uniform array[array_size];
        for (size_t i = 0; i < array_size; ++i) {
            array[i].i = static_cast<int>(i);
            array[i].d = 10.0 * static_cast<double>(i);
        }
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, &array[0], (((AMP_BYTE *)&array[3]) + 3), sizeof(non_uniform), sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[0].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[1].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[2].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range));  
    }
    
    
    
    TEST(multi_item_init_interleaved_without_end_gap)
    {
        size_t const array_size = 10;
        struct non_uniform array[array_size];
        for (size_t i = 0; i < array_size; ++i) {
            array[i].i = static_cast<int>(i);
            array[i].d = 10.0 * static_cast<double>(i);
        }
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, &array[0], &array[3], sizeof(non_uniform), sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[0].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[1].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[2].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range));  
    }
    
    
    
    TEST(empty_range_with_zero_step_with_end_gap)
    {
        size_t const array_size = 10;
        struct non_uniform array[array_size];
        for (size_t i = 0; i < array_size; ++i) {
            array[i].i = static_cast<int>(i);
            array[i].d = 10.0 * static_cast<double>(i);
        }
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, 
                                         &array[0], 
                                         (((AMP_BYTE *)&array[0]) + 3), 
                                         0, 
                                         sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range));  
    }
    
    

    TEST(empty_range_with_zero_step_without_end_gap)
    {
        size_t const array_size = 10;
        struct non_uniform array[array_size];
        for (size_t i = 0; i < array_size; ++i) {
            array[i].i = static_cast<int>(i);
            array[i].d = 10.0 * static_cast<double>(i);
        }
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, 
                                         &array[0], 
                                         &array[0], 
                                         0, 
                                         sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range));  
    }
    
    
    
    TEST(empty_range_with_multi_step_with_end_gap)
    {
        size_t const array_size = 10;
        struct non_uniform array[array_size];
        for (size_t i = 0; i < array_size; ++i) {
            array[i].i = static_cast<int>(i);
            array[i].d = 10.0 * static_cast<double>(i);
        }
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, 
                                         &array[0], 
                                         (((AMP_BYTE *)&array[0]) + 3), 
                                         2 * sizeof(struct non_uniform), 
                                         sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range)); 
    }
    
    
    
    TEST(empty_range_with_multi_step_without_end_gap)
    {
        size_t const array_size = 10;
        struct non_uniform array[array_size];
        for (size_t i = 0; i < array_size; ++i) {
            array[i].i = static_cast<int>(i);
            array[i].d = 10.0 * static_cast<double>(i);
        }
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, 
                                         &array[0], 
                                         &array[0], 
                                         2 * sizeof(struct non_uniform), 
                                         sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range)); 
    }

    
    
    
    TEST(multi_item_range_with_zero_step_with_end_gap)
    {
        size_t const array_size = 10;
        struct non_uniform array[array_size];
        for (size_t i = 0; i < array_size; ++i) {
            array[i].i = static_cast<int>(i);
            array[i].d = 10.0 * static_cast<double>(i);
        }
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range,
                                         &array[0], 
                                         (((AMP_BYTE *)&array[3]) + 3), 
                                         0, 
                                         sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[0].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[0].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[0].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range)); 
    }
    
    
    
    TEST(multi_item_range_with_zero_step_without_end_gap)
    {
        size_t const array_size = 10;
        struct non_uniform array[array_size];
        for (size_t i = 0; i < array_size; ++i) {
            array[i].i = static_cast<int>(i);
            array[i].d = 10.0 * static_cast<double>(i);
        }
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range,
                                         &array[0], 
                                         &array[3], 
                                         0, 
                                         sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[0].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[0].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[0].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range)); 
    }
    
    
    
    TEST(multi_item_range_with_multi_step_with_end_gap)
    {
        size_t const array_size = 10;
        struct non_uniform array[array_size];
        for (size_t i = 0; i < array_size; ++i) {
            array[i].i = static_cast<int>(i);
            array[i].d = 10.0 * static_cast<double>(i);
        }
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range,
                                         &array[0], 
                                         (((AMP_BYTE *)&array[5]) + 3), 
                                         2 * sizeof(struct non_uniform), 
                                         sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[0].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[2].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[4].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range)); 
    }
    
    

    TEST(multi_item_range_with_multi_step_without_end_gap)
    {
        size_t const array_size = 10;
        struct non_uniform array[array_size];
        for (size_t i = 0; i < array_size; ++i) {
            array[i].i = static_cast<int>(i);
            array[i].d = 10.0 * static_cast<double>(i);
        }
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range,
                                         &array[0], 
                                         &array[5], 
                                         2 * sizeof(struct non_uniform), 
                                         sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[0].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[2].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_empty(&range));  
        
        CHECK_EQUAL(array[4].i, *((int *)amp_raw_byte_range_get_front(&range)));
        
        amp_raw_byte_range_advance_front(&range);
        
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_empty(&range)); 
    }
    
    
    
    TEST(empty_range_with_end_gap_advanceable_count)
    {
        size_t const ia_size = 10;
        int ia[ia_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, &ia[0], ((AMP_BYTE *)&ia[0]) + 1, sizeof(int), sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval); 
        
        CHECK_EQUAL(static_cast<size_t>(0), amp_raw_byte_range_advanceable_count(&range));
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_infinitely_advanceable(&range));
    }
    
    
    
    TEST(empty_range_without_end_gap_advanceable_count)
    {
        size_t const ia_size = 10;
        int ia[ia_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, &ia[0], &ia[0], 0, sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval); 
        
        CHECK_EQUAL(static_cast<size_t>(0), amp_raw_byte_range_advanceable_count(&range));
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_infinitely_advanceable(&range));
    }
    
    
    
    TEST(multi_item_range_with_end_gap_non_infinite_advanceable_count)
    {
        size_t const ia_size = 10;
        int ia[ia_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, &ia[0], ((AMP_BYTE *)&ia[3]) + 1, sizeof(int), sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval); 
        
        CHECK_EQUAL(static_cast<size_t>(3), amp_raw_byte_range_advanceable_count(&range));
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_infinitely_advanceable(&range));
    }
    
    
    
    TEST(multi_item_range_without_end_gap_non_infinite_advanceable_count)
    {
        size_t const ia_size = 10;
        int ia[ia_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, &ia[0], &ia[3], sizeof(int), sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval); 
        
        CHECK_EQUAL(static_cast<size_t>(3), amp_raw_byte_range_advanceable_count(&range));
        CHECK_EQUAL(AMP_FALSE, amp_raw_byte_range_is_infinitely_advanceable(&range));
    }
    
    
    
    TEST(multi_item_range_with_end_gap_infinite_advanceable_count)
    {
        size_t const ia_size = 10;
        int ia[ia_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, &ia[0], ((AMP_BYTE *)&ia[3]) + 1, 0, sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval); 
        
        CHECK_EQUAL(~(static_cast<size_t>(0)), amp_raw_byte_range_advanceable_count(&range));
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_infinitely_advanceable(&range));
    }
    
    
    
    TEST(multi_item_range_without_end_gap_infinite_advanceable_count)
    {
        size_t const ia_size = 10;
        int ia[ia_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        struct amp_raw_byte_range_s range;
        int retval = amp_raw_byte_range_init(&range, &ia[0], &ia[3], 0, sizeof(int));
        CHECK_EQUAL(AMP_SUCCESS, retval); 
        
        CHECK_EQUAL(~(static_cast<size_t>(0)), amp_raw_byte_range_advanceable_count(&range));
        CHECK_EQUAL(AMP_TRUE, amp_raw_byte_range_is_infinitely_advanceable(&range));
    }
    
    
} // SUITE(amp_data_slice)



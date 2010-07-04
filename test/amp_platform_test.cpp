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
 * Unit tests for amp_platform
 */

#include <UnitTest++.h>


#include <cassert>
#include <cstddef>


#include <amp/amp_stddef.h>
#include <amp/amp_return_code.h>
#include <amp/amp_memory.h>
#include <amp/amp_platform.h>



namespace {
    
    class amp_platform_test_fixture {
    public:
        
        amp_platform_test_fixture()
        :   platform(AMP_PLATFORM_UNINITIALIZED)
        {
            int const error_code = amp_platform_create(&platform,
                                                       AMP_DEFAULT_ALLOCATOR,
                                                       &amp_default_alloc,
                                                       &amp_default_dealloc);
            assert(AMP_SUCCESS == error_code);
            
        }
        
        
        virtual ~amp_platform_test_fixture()
        {
            int const error_code = amp_platform_destroy(&platform,
                                                        AMP_DEFAULT_ALLOCATOR,
                                                        &amp_default_dealloc);
            
            assert(AMP_SUCCESS == error_code);
            platform = AMP_PLATFORM_UNINITIALIZED;
        }
        
        
        amp_platform_t platform;
        
    private:
        amp_platform_test_fixture(amp_platform_test_fixture const&); // =0
        amp_platform_test_fixture& operator=(amp_platform_test_fixture const&); // =0
    };
    
    
    
    struct statistics_collecting_allocator {
      
        std::size_t allocated_memory;
        std::size_t deallocated_memory;
    };
    
    
    
    void* statistics_collecting_alloc(void* context,
                                      std::size_t size)
    {
        struct statistics_collecting_allocator* ctxt = (struct statistics_collecting_allocator*)context;
        
        ctxt->allocated_memory += size;
        
        std::size_t* ptr = (std::size_t*)amp_default_alloc(AMP_DEFAULT_ALLOCATOR, 
                                                           size + sizeof(std::size_t));
        
        *ptr = size;
        
        return (void*)(ptr + 1);
    }
    
    
    
    int statistics_collecting_dealloc(void* context,
                                      void* pointer_to_dealloc)
    {
        struct statistics_collecting_allocator* ctxt = (struct statistics_collecting_allocator*)context;
        
        
        std::size_t* ptr = (std::size_t*)pointer_to_dealloc;
        ptr = ptr - 1;
        
        ctxt->deallocated_memory += *ptr;
        
        int const retval = amp_default_dealloc(AMP_DEFAULT_ALLOCATOR, ptr);
        
        return retval;
    }
    
    
    
} // anonymous namespace


SUITE(amp_platform)
{

    TEST_FIXTURE(amp_platform_test_fixture, get_core_count_no_argument_change_on_error)
    {
        size_t const count_init_value = 666;
        size_t count_that_must_not_be_touched_on_error = count_init_value;
        int const error_code = amp_platform_get_installed_core_count(platform, &count_that_must_not_be_touched_on_error);
        
        if (AMP_SUCCESS != error_code) {
            CHECK(count_init_value == count_that_must_not_be_touched_on_error);
        }
    }
    
    
    
    TEST_FIXTURE(amp_platform_test_fixture, get_core_active_count_no_argument_change_on_error)
    {
        size_t const count_init_value = 666;
        size_t count_that_must_not_be_touched_on_error = count_init_value;
        int const error_code = amp_platform_get_active_core_count(platform, &count_that_must_not_be_touched_on_error);
        
        if (AMP_SUCCESS != error_code) {
            CHECK(count_init_value == count_that_must_not_be_touched_on_error);
        }
    }
    
    
    
    TEST_FIXTURE(amp_platform_test_fixture, get_hwthread_count_no_argument_change_on_error)
    {
        size_t const count_init_value = 666;
        size_t count_that_must_not_be_touched_on_error = count_init_value;
        int const error_code = amp_platform_get_installed_hwthread_count(platform, &count_that_must_not_be_touched_on_error);
        
        if (AMP_SUCCESS != error_code) {
            CHECK(count_init_value == count_that_must_not_be_touched_on_error);
        }
    }
    
    
    
    TEST_FIXTURE(amp_platform_test_fixture, get_active_hwthread_count_no_argument_change_on_error)
    {
        size_t const count_init_value = 666;
        size_t count_that_must_not_be_touched_on_error = count_init_value;
        int const error_code = amp_platform_get_active_hwthread_count(platform, &count_that_must_not_be_touched_on_error);
        
        if (AMP_SUCCESS != error_code) {
            CHECK(count_init_value == count_that_must_not_be_touched_on_error);
        }
    }
    
    
    
    TEST_FIXTURE(amp_platform_test_fixture, active_core_count_lesser_or_equal_than_max_core_count)
    {
        size_t active_count = 0;
        size_t max_count = 0;
        
        int error_code = amp_platform_get_installed_core_count(platform, &max_count);
        assert(AMP_SUCCESS == error_code || ENOSYS == error_code);
        
        error_code = amp_platform_get_active_core_count(platform, &active_count);
        assert(AMP_SUCCESS == error_code || ENOSYS == error_code);
        
        CHECK(active_count <= max_count);
    }
    
    

    TEST_FIXTURE(amp_platform_test_fixture, active_hwthread_count_lesser_or_equal_than_max_hwthread_count)
    {
        size_t active_count = 0;
        size_t max_count = 0;
        
        int error_code = amp_platform_get_installed_hwthread_count(platform, &max_count);
        assert(AMP_SUCCESS == error_code || ENOSYS == error_code);
        
        error_code = amp_platform_get_active_hwthread_count(platform, &active_count);
        assert(AMP_SUCCESS == error_code || ENOSYS == error_code);
        
        CHECK(active_count <= max_count);
    }

    
    
    TEST(memory_allocation_and_deallocation)
    {
        amp_platform_t platform;
        struct statistics_collecting_allocator allocator = {0, 0};
        
        
        int error_code = amp_platform_create(&platform,
                                             &allocator,
                                             &statistics_collecting_alloc,
                                             &statistics_collecting_dealloc);
        assert(AMP_SUCCESS == error_code);
        
        size_t dummy_count = 0;
        error_code = amp_platform_get_installed_core_count(platform, &dummy_count);
        assert(AMP_SUCCESS == error_code || ENOSYS == error_code);
        error_code = amp_platform_get_active_core_count(platform, &dummy_count);
        assert(AMP_SUCCESS == error_code || ENOSYS == error_code);
        error_code = amp_platform_get_installed_hwthread_count(platform, &dummy_count);
        assert(AMP_SUCCESS == error_code || ENOSYS == error_code);
        error_code = amp_platform_get_active_hwthread_count(platform, &dummy_count);
        assert(AMP_SUCCESS == error_code || ENOSYS == error_code);
        
        error_code = amp_platform_destroy(&platform,
                                          &allocator,
                                          &statistics_collecting_dealloc);
        assert(AMP_SUCCESS == error_code);
        
        CHECK_EQUAL(allocator.allocated_memory, allocator.deallocated_memory);
    }
    
    
    
} // SUITE(amp_platform)




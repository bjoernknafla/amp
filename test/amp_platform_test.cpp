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
                                                       AMP_DEFAULT_ALLOCATOR);
            assert(AMP_SUCCESS == error_code);
            (void)error_code;
            
        }
        
        
        virtual ~amp_platform_test_fixture()
        {
            int const error_code = amp_platform_destroy(&platform,
                                                        AMP_DEFAULT_ALLOCATOR);
            
            assert(AMP_SUCCESS == error_code);
            (void)error_code;
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
                                      std::size_t size,
                                      char const* filename,
                                      int line)
    {
        (void)filename;
        (void)line;
        
        struct statistics_collecting_allocator* ctxt = (struct statistics_collecting_allocator*)context;
        
        ctxt->allocated_memory += size;
        
        std::size_t* ptr = (std::size_t*)AMP_ALLOC(AMP_DEFAULT_ALLOCATOR, 
                                                   size + sizeof(std::size_t));
        
        *ptr = size;
        
        return (void*)(ptr + 1);
    }
    
    
    
    void* statistics_collecting_calloc(void* context,
                                       std::size_t elem_count,
                                       std::size_t elem_size_in_bytes,
                                       char const* filename,
                                       int line)
    {
        (void)filename;
        (void)line;
        
        struct statistics_collecting_allocator* ctxt = (struct statistics_collecting_allocator*)context;
        
        std::size_t const size = elem_count * elem_size_in_bytes;
        
        ctxt->allocated_memory += size;
        
        std::size_t* ptr = (std::size_t*)AMP_ALLOC(AMP_DEFAULT_ALLOCATOR, 
                                                   size + sizeof(std::size_t));
        
        *ptr = size;
        
        return (void*)(ptr + 1);
    }
    
    
    
    int statistics_collecting_dealloc(void* context,
                                      void* pointer_to_dealloc,
                                      char const* filename,
                                      int line)
    {
        (void)filename;
        (void)line;
        
        struct statistics_collecting_allocator* ctxt = (struct statistics_collecting_allocator*)context;
        
        
        std::size_t* ptr = (std::size_t*)pointer_to_dealloc;
        ptr = ptr - 1;
        
        ctxt->deallocated_memory += *ptr;
        
        int const retval = AMP_DEALLOC(AMP_DEFAULT_ALLOCATOR, ptr);
        
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
        assert(AMP_SUCCESS == error_code || AMP_UNSUPPORTED == error_code);
        
        error_code = amp_platform_get_active_core_count(platform, &active_count);
        assert(AMP_SUCCESS == error_code || AMP_UNSUPPORTED == error_code);
        
        CHECK(active_count <= max_count);
    }
    
    

    TEST_FIXTURE(amp_platform_test_fixture, active_hwthread_count_lesser_or_equal_than_max_hwthread_count)
    {
        size_t active_count = 0;
        size_t max_count = 0;
        
        int error_code = amp_platform_get_installed_hwthread_count(platform, &max_count);
        assert(AMP_SUCCESS == error_code || AMP_UNSUPPORTED == error_code);
        
        error_code = amp_platform_get_active_hwthread_count(platform, &active_count);
        assert(AMP_SUCCESS == error_code || AMP_UNSUPPORTED == error_code);
        
        CHECK(active_count <= max_count);
    }

    
    
    TEST_FIXTURE(amp_platform_test_fixture, active_hwthread_count_greater_or_equal_to_active_core_count_if_both_supported)
    {
        size_t active_hwthread_count = 0;
        size_t active_core_count = 0;
        
        int retcode_hwthread = AMP_UNSUPPORTED;
        int retcode_core = AMP_UNSUPPORTED;
        
        retcode_hwthread = amp_platform_get_active_hwthread_count(platform,
                                                                 &active_hwthread_count);
        CHECK(AMP_SUCCESS == retcode_hwthread || AMP_UNSUPPORTED == retcode_hwthread);
        
        retcode_core = amp_platform_get_active_core_count(platform,
                                                          &active_core_count);
        CHECK(AMP_SUCCESS == retcode_core || AMP_UNSUPPORTED == retcode_core);
        
        if (AMP_SUCCESS == retcode_hwthread) {
            CHECK( active_hwthread_count >= active_core_count);
        } else {
            CHECK( active_hwthread_count <= active_core_count);
        }
    }
    
    
    
    TEST_FIXTURE(amp_platform_test_fixture, installed_hwthread_count_greater_or_equal_to_installed_core_count_if_both_supported)
    {
        size_t installed_hwthread_count = 0;
        size_t installed_core_count = 0;
        
        int retcode_hwthread = AMP_UNSUPPORTED;
        int retcode_core = AMP_UNSUPPORTED;
        
        retcode_hwthread = amp_platform_get_installed_hwthread_count(platform,
                                                                     &installed_hwthread_count);
        CHECK(AMP_SUCCESS == retcode_hwthread || AMP_UNSUPPORTED == retcode_hwthread);
        
        retcode_core = amp_platform_get_installed_core_count(platform,
                                                             &installed_core_count);
        CHECK(AMP_SUCCESS == retcode_core || AMP_UNSUPPORTED == retcode_core);
        
        if (AMP_SUCCESS == retcode_hwthread) {
            CHECK( installed_hwthread_count >= installed_core_count);
        } else {
            CHECK( installed_hwthread_count <= installed_core_count);
        }
    }
    
    
    
    TEST_FIXTURE(amp_platform_test_fixture, concurrency_level) 
    {
        size_t installed_hwthread_count = 0;
        size_t active_hwthread_count = 0;
        size_t installed_core_count = 0;
        size_t active_core_count = 0;
        
        int retcode = amp_platform_get_installed_hwthread_count(platform,
                                                                &installed_hwthread_count);
        assert(AMP_SUCCESS == retcode || AMP_UNSUPPORTED == retcode);
        
        retcode = amp_platform_get_active_hwthread_count(platform,
                                                         &active_hwthread_count);
        assert(AMP_SUCCESS == retcode || AMP_UNSUPPORTED == retcode);
        
        retcode = amp_platform_get_installed_core_count(platform,
                                                        &installed_core_count);
        assert(AMP_SUCCESS == retcode || AMP_UNSUPPORTED == retcode);
        
        retcode = amp_platform_get_active_core_count(platform,
                                                     &active_core_count);
        assert(AMP_SUCCESS == retcode || AMP_UNSUPPORTED == retcode);
        
        size_t const max_count = std::max(std::max(installed_hwthread_count, active_hwthread_count), 
                                          std::max(installed_core_count, active_core_count));
        
        size_t concurrency_level = 0;
        retcode = amp_platform_get_concurrency_level(platform, 
                                                     &concurrency_level);
        assert(AMP_SUCCESS == retcode || AMP_UNSUPPORTED == retcode);
        
        CHECK_EQUAL(max_count, concurrency_level);
    }
    
    
    
    TEST(memory_allocation_and_deallocation)
    {
        amp_platform_t platform;
        struct statistics_collecting_allocator allocator_context = {0, 0};
        
        
        amp_allocator_t allocator = AMP_ALLOCATOR_UNINITIALIZED;
        int error_code = amp_allocator_create(&allocator,
                                         AMP_DEFAULT_ALLOCATOR,
                                         &allocator_context,
                                         &statistics_collecting_alloc, 
                                         &statistics_collecting_calloc,
                                         &statistics_collecting_dealloc);
        assert(AMP_SUCCESS == error_code);
        
        
        error_code = amp_platform_create(&platform,
                                             allocator);
        assert(AMP_SUCCESS == error_code);
        
        size_t dummy_count = 0;
        error_code = amp_platform_get_installed_core_count(platform, &dummy_count);
        assert(AMP_SUCCESS == error_code || AMP_UNSUPPORTED == error_code);
        error_code = amp_platform_get_active_core_count(platform, &dummy_count);
        assert(AMP_SUCCESS == error_code || AMP_UNSUPPORTED == error_code);
        error_code = amp_platform_get_installed_hwthread_count(platform, &dummy_count);
        assert(AMP_SUCCESS == error_code || AMP_UNSUPPORTED == error_code);
        error_code = amp_platform_get_active_hwthread_count(platform, &dummy_count);
        assert(AMP_SUCCESS == error_code || AMP_UNSUPPORTED == error_code);
        
        error_code = amp_platform_destroy(&platform,
                                          allocator);
        assert(AMP_SUCCESS == error_code);
        
        CHECK_EQUAL(allocator_context.allocated_memory, allocator_context.deallocated_memory);
        
        error_code = amp_allocator_destroy(&allocator,
                                           AMP_DEFAULT_ALLOCATOR);
        assert(AMP_SUCCESS == error_code);
    }
    
    
    
} // SUITE(amp_platform)




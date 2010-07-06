/*
 *  amp_raw_barrier_test.cpp
 *  amp
 *
 *  Created by Björn Knafla on 15.05.10.
 *  Copyright 2010 Bjoern Knafla Parallelization + AI + Gamedev Consulting. All rights reserved.
 *
 */


#include <UnitTest++.h>

#include <cassert>
#include <cstddef>
#include <algorithm>
#include <vector>

#include <amp/amp.h>
#include <amp/amp_return_code.h>



SUITE(amp_barrier)
{
    /* TODO: @todo Remove the asserts for conditions that are also handled
     *             to enable testing of bad usage of barriers.
    TEST(erroneous_init)
    {
        int errc = amp_barrier_create(NULL, 
                                      5,
                                      AMP_DEFAULT_ALLOCATOR,
                                      &amp_default_alloc,
                                      &amp_default_dealloc);
        CHECK_EQUAL(EINVAL, errc);
        
        amp_barrier_t barrier = AMP_BARRIER_UNINITIALIZED;
        errc = amp_barrier_create(&barrier, 
                                  0,
                                  AMP_DEFAULT_ALLOCATOR,
                                  &amp_default_alloc,
                                  &amp_default_dealloc);
        CHECK_EQUAL(EINVAL, errc);
    }
    */

    TEST(init_and_finalize) 
    {
        amp_barrier_t barrier = AMP_BARRIER_UNINITIALIZED;
        int errc = amp_barrier_create(&barrier,
                                      AMP_DEFAULT_ALLOCATOR,
                                      1);
        CHECK_EQUAL(AMP_SUCCESS, errc);
        
        errc = amp_barrier_wait(barrier);
        CHECK_EQUAL(AMP_BARRIER_SERIAL_THREAD, errc);
        
        errc = amp_barrier_destroy(&barrier,
                                   AMP_DEFAULT_ALLOCATOR);
        CHECK_EQUAL(AMP_SUCCESS, errc);
        barrier = AMP_BARRIER_UNINITIALIZED;
    }


    
    namespace {
        
        enum parallel_wait_state {
            parallel_wait_state_initialized,
            parallel_wait_state_before_barrier,
            parallel_wait_state_behind_barrier_normal,
            parallel_wait_state_behind_barrier_serial,
            parallel_wait_state_behind_barrier_error
        };
        typedef enum parallel_wait_state parallel_wait_state_t;
        
        struct parallel_wait_thread_context {
            amp_barrier_t* shared_barrier;
            parallel_wait_state_t state;
        };
        typedef struct parallel_wait_thread_context* parallel_wait_thread_context_t;
        
        void parallel_wait_thread_func(void* ctxt);
        void parallel_wait_thread_func(void* ctxt)
        {
            parallel_wait_thread_context_t context = (parallel_wait_thread_context_t)ctxt;
            
            assert(parallel_wait_state_initialized == context->state);
            
            context->state = parallel_wait_state_before_barrier;
            
            int const rc = amp_barrier_wait(*context->shared_barrier);
            assert(AMP_SUCCESS == rc || AMP_BARRIER_SERIAL_THREAD == rc);
            
            switch (rc)
            {
                case AMP_SUCCESS:
                    context->state = parallel_wait_state_behind_barrier_normal;
                    break;
                case AMP_BARRIER_SERIAL_THREAD:
                    context->state = parallel_wait_state_behind_barrier_serial;
                    break;
                default:
                    context->state = parallel_wait_state_behind_barrier_error;
            }
            
        }
        
    } // anonymous namespace
    
    
    TEST(parallel_wait) 
    {
        unsigned int const thread_count = 16;
        
        amp_barrier_t barrier = AMP_BARRIER_UNINITIALIZED;
        int retval = amp_barrier_create(&barrier,
                                        AMP_DEFAULT_ALLOCATOR,
                                        thread_count + 1 /* Plus one for the main thread */
                                        );
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        struct parallel_wait_thread_context prototype_context = {
            &barrier,
            parallel_wait_state_initialized
        };
        std::vector<struct parallel_wait_thread_context> thread_contexts(thread_count, prototype_context);
        
        amp_thread_array_t threads = AMP_THREAD_ARRAY_UNINITIALIZED;
        retval = amp_thread_array_create(&threads,
                                         AMP_DEFAULT_ALLOCATOR,
                                         thread_count);
        assert(AMP_SUCCESS == retval);
        
        for (unsigned int i = 0; i < thread_count; ++i) {
            retval = amp_thread_array_configure(threads,
                                                i,
                                                1,
                                                &thread_contexts[i],
                                                &parallel_wait_thread_func);
            assert(AMP_SUCCESS == retval);
        }
        
        std::size_t joinable_count = 0;
        retval = amp_thread_array_launch_all(threads, &joinable_count);
        assert(AMP_SUCCESS == retval);
        assert(thread_count == static_cast<std::size_t>(joinable_count));
        
        
        // Main thread also needs to wait on the barrier so all threads can
        // pass it and finish executing.
        retval = amp_barrier_wait(barrier);
        CHECK(AMP_SUCCESS == retval || AMP_BARRIER_SERIAL_THREAD == retval);
        
        
        retval = amp_thread_array_join_all(threads, &joinable_count);
        assert(AMP_SUCCESS == retval);
        assert(0 == joinable_count);
        
        retval = amp_thread_array_destroy(&threads,
                                          AMP_DEFAULT_ALLOCATOR);
        assert(AMP_SUCCESS == retval);
        threads = AMP_THREAD_ARRAY_UNINITIALIZED;
        
        retval = amp_barrier_destroy(&barrier,
                                     AMP_DEFAULT_ALLOCATOR);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        int serial_state_counter = 0;
        
        for (unsigned int i = 0; i < thread_count; ++i) {
            
            parallel_wait_state_t state = thread_contexts[i].state;
            
            if (parallel_wait_state_behind_barrier_serial == state) {
                ++serial_state_counter;
            }
            
            CHECK(parallel_wait_state_behind_barrier_normal == state 
                  || parallel_wait_state_behind_barrier_serial == state);
        }
        
        CHECK(1 >= serial_state_counter);
    }
    
    
    
    namespace {
        
        
        struct no_passing {
          
            amp_barrier_t *shared_barrier;
            amp_mutex_t *shared_counter_mutex;
            amp_semaphore_t *shared_wakeup_sema;
            
            int before_first_wait;
            int after_first_wait;
            int after_second_wait;
            
            int* shared_counter;
            int observed_counter_before_end;
        };
        
        
        void wait_wake_others_wait(void* ctxt);
        void wait_wake_others_wait(void* ctxt)
        {
            struct no_passing* context = (struct no_passing*)ctxt;
            
            int observed_counter_before_end = 0;
            
            context->before_first_wait = 1;
            
            int rc = amp_barrier_wait(*context->shared_barrier);
            assert(AMP_SUCCESS == rc || AMP_BARRIER_SERIAL_THREAD == rc);
            
            context->after_first_wait = 1;
            
            rc = amp_semaphore_signal(*context->shared_wakeup_sema);
            assert(AMP_SUCCESS == rc);
            
            rc = amp_barrier_wait(*context->shared_barrier);
            assert(AMP_SUCCESS == rc || AMP_BARRIER_SERIAL_THREAD == rc);
            
            rc = amp_mutex_lock(*context->shared_counter_mutex);
            assert(AMP_SUCCESS == rc);
            {
                observed_counter_before_end = (*context->shared_counter)++;
            }
            rc = amp_mutex_unlock(*context->shared_counter_mutex);
            assert(AMP_SUCCESS == rc);
            
            context->after_second_wait = 1;
            context->observed_counter_before_end = observed_counter_before_end;
            
        }
        
        
        void sleep_then_wait(void* ctxt);
        void sleep_then_wait(void* ctxt)
        {
            struct no_passing* context = (struct no_passing*)ctxt;
            
            int observed_counter_before_end = 0;
            
            int rc = amp_semaphore_wait(*context->shared_wakeup_sema);
            assert(AMP_SUCCESS == rc);
            
            context->before_first_wait = 1;
            
            rc = amp_barrier_wait(*context->shared_barrier);
            assert(AMP_SUCCESS == rc || AMP_BARRIER_SERIAL_THREAD == rc);
            
            rc = amp_mutex_lock(*context->shared_counter_mutex);
            assert(AMP_SUCCESS == rc);
            {
                observed_counter_before_end = (*context->shared_counter)++;
            }
            rc = amp_mutex_unlock(*context->shared_counter_mutex);
            assert(AMP_SUCCESS == rc);
            
            context->after_first_wait = 1;
            context->observed_counter_before_end = observed_counter_before_end;
            
        }
        
        
        void wait_only(void* ctxt);
        void wait_only(void* ctxt)
        {
            struct no_passing* context = (struct no_passing*)ctxt;
            
            int observed_counter_before_end = 0;
            
            context->before_first_wait = 1;
            
            int rc = amp_barrier_wait(*context->shared_barrier);
            assert(AMP_SUCCESS == rc || AMP_BARRIER_SERIAL_THREAD == rc);
            
            rc = amp_mutex_lock(*context->shared_counter_mutex);
            assert(AMP_SUCCESS == rc);
            {
                observed_counter_before_end = (*context->shared_counter)++;
            }
            rc = amp_mutex_unlock(*context->shared_counter_mutex);
            assert(AMP_SUCCESS == rc);
            
            context->after_first_wait = 1;
            context->observed_counter_before_end = observed_counter_before_end;
        }
        
        
        
    } // anonymous namespace
    
    
    TEST(parallel_waiting_and_no_passing_of_new_waiters_during_wake_up_of_old)
    {
        amp_platform_t platform = AMP_PLATFORM_UNINITIALIZED;
        int retval = amp_platform_create(&platform,
                                         AMP_DEFAULT_ALLOCATOR);
        assert(AMP_SUCCESS == retval);
        
        std::size_t concurrency_level = 0;
        retval = amp_platform_get_installed_hwthread_count(platform,
                                                           &concurrency_level);
        assert(AMP_SUCCESS == retval || AMP_UNSUPPORTED == retval);
        if (AMP_UNSUPPORTED == retval) {
            retval = amp_platform_get_installed_core_count(platform,
                                                           &concurrency_level);
            assert(AMP_SUCCESS == retval || AMP_UNSUPPORTED == retval);
            if (AMP_UNSUPPORTED == retval) {
                concurrency_level = 8;
            }
        }
        
        concurrency_level = std::max(concurrency_level, static_cast<std::size_t>(8));
        
        retval = amp_platform_destroy(&platform,
                                      AMP_DEFAULT_ALLOCATOR);
        assert(AMP_SUCCESS == retval);
        
        
        std::size_t const a_third_of_thread_count = concurrency_level;
        std::size_t const thread_count = a_third_of_thread_count * 3;
        
        int shared_counter = 0;
        
        amp_barrier_t shared_barrier = AMP_BARRIER_UNINITIALIZED;
        amp_mutex_t shared_counter_mutex = AMP_MUTEX_UNINITIALIZED;
        amp_semaphore_t shared_wakeup_sema = AMP_SEMAPHORE_UNINITIALIZED;
    
        retval = amp_barrier_create(&shared_barrier,
                                    AMP_DEFAULT_ALLOCATOR,
                                    static_cast<amp_barrier_count_t>(a_third_of_thread_count * 2));
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        retval = amp_mutex_create(&shared_counter_mutex,
                                  AMP_DEFAULT_ALLOCATOR);
        assert(AMP_SUCCESS == retval);
                                      
        retval = amp_semaphore_create(&shared_wakeup_sema,
                                      AMP_DEFAULT_ALLOCATOR,
                                      0);
        assert(AMP_SUCCESS == retval);
        
        amp_thread_array_t threads = AMP_THREAD_ARRAY_UNINITIALIZED;
        retval = amp_thread_array_create(&threads,
                                         AMP_DEFAULT_ALLOCATOR,
                                         thread_count);
        assert(AMP_SUCCESS == retval);
        
        
        
        
        
        struct no_passing thread_context_prototype = {
            &shared_barrier,
            &shared_counter_mutex,
            &shared_wakeup_sema,
            0,
            0,
            0,
            &shared_counter,
            0
        };
        
        std::vector<struct no_passing> thread_contexts(thread_count, 
                                                       thread_context_prototype);
        
        for (std::size_t i = 0; i < a_third_of_thread_count; ++i) {
            retval = amp_thread_array_configure(threads,
                                                i,
                                                1,
                                                &thread_contexts[i],
                                                &wait_wake_others_wait);
            assert(AMP_SUCCESS == retval);
        }
        for (std::size_t i = a_third_of_thread_count; i < 2 * a_third_of_thread_count; ++i) {
            retval = amp_thread_array_configure(threads,
                                                i,
                                                1,
                                                &thread_contexts[i],
                                                &wait_only);
            assert(AMP_SUCCESS == retval);
        }
        for (std::size_t i = 2 * a_third_of_thread_count; i < 3 * a_third_of_thread_count; ++i) {
            retval = amp_thread_array_configure(threads,
                                                i,
                                                1,
                                                &thread_contexts[i],
                                                &sleep_then_wait);
            assert(AMP_SUCCESS == retval);
        }
        
        std::size_t joinable_count = 0;
        retval = amp_thread_array_launch_all(threads,
                                             &joinable_count);
        assert(AMP_SUCCESS == retval);
        assert(thread_count == joinable_count);
        
        // Nothing to do just wait till all threads are done.
        
        retval = amp_thread_array_join_all(threads,
                                             &joinable_count);
        assert(AMP_SUCCESS == retval);
        assert(0 == joinable_count);
        

        retval = amp_barrier_destroy(&shared_barrier,
                                     AMP_DEFAULT_ALLOCATOR);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        shared_barrier = AMP_BARRIER_UNINITIALIZED;
        
        retval = amp_mutex_destroy(&shared_counter_mutex,
                                   AMP_DEFAULT_ALLOCATOR);
        assert(AMP_SUCCESS == retval);
        shared_counter_mutex = AMP_MUTEX_UNINITIALIZED;
        
        retval = amp_semaphore_destroy(&shared_wakeup_sema,
                                       AMP_DEFAULT_ALLOCATOR);
        assert(AMP_SUCCESS == retval);
        shared_wakeup_sema = AMP_SEMAPHORE_UNINITIALIZED;
        
        retval = amp_thread_array_destroy(&threads,
                                          AMP_DEFAULT_ALLOCATOR);
        assert(AMP_SUCCESS == retval);
        threads = AMP_THREAD_ARRAY_UNINITIALIZED;

        
        CHECK_EQUAL(static_cast<std::size_t>(shared_counter),thread_count);
        for (std::size_t i = 0; i < a_third_of_thread_count; ++i) {
            CHECK(static_cast<std::size_t>(thread_contexts[i].observed_counter_before_end) >= a_third_of_thread_count);
            CHECK(static_cast<std::size_t>(thread_contexts[i].observed_counter_before_end) < 3 * a_third_of_thread_count);
        }
        for (std::size_t i = a_third_of_thread_count; i < 2 * a_third_of_thread_count; ++i) {
            CHECK(static_cast<std::size_t>(thread_contexts[i].observed_counter_before_end) < a_third_of_thread_count);
        }
        for (std::size_t i = 2 * a_third_of_thread_count; i < 3 * a_third_of_thread_count; ++i) {
            CHECK(static_cast<std::size_t>(thread_contexts[i].observed_counter_before_end) >= a_third_of_thread_count);
            CHECK(static_cast<std::size_t>(thread_contexts[i].observed_counter_before_end) < 3 * a_third_of_thread_count);
        }
        
    }
    
    
} // SUITE(amp_raw_barrier)

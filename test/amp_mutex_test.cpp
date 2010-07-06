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
 * Unit tests for amp_raw_mutex.
 */


#include <UnitTest++.h>



#include <assert.h>
#include <stddef.h>

#include <amp/amp_stddef.h>
#include <amp/amp_return_code.h>
#include <amp/amp_thread.h>
#include <amp/amp_thread_array.h>
#include <amp/amp_mutex.h>


SUITE(amp_mutex)
{
    
    
    TEST(single_thread_init_lock_unlock_finalize)
    {
        amp_mutex_t mutex;
        
        int retval = amp_mutex_create(&mutex,
                                      AMP_DEFAULT_ALLOCATOR);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        {
            int const milliseconds = 50;
            UNITTEST_TIME_CONSTRAINT(milliseconds);
            
            retval = amp_mutex_lock(mutex);
            CHECK_EQUAL(AMP_SUCCESS, retval);
            {
                // Critical section. Nothing to do.
            }
            retval = amp_mutex_unlock(mutex);
            CHECK_EQUAL(AMP_SUCCESS, retval);
        }
        
        retval = amp_mutex_destroy(&mutex,
                                   AMP_DEFAULT_ALLOCATOR);
        CHECK_EQUAL(AMP_SUCCESS, retval);
    }
    
    
    
    TEST(single_thread_init_trylock_unlock_finalize)
    {
        amp_mutex_t mutex;
        
        int retval = amp_mutex_create(&mutex,
                                      AMP_DEFAULT_ALLOCATOR);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        {
            int const milliseconds = 50;
            UNITTEST_TIME_CONSTRAINT(milliseconds);
            
            retval = amp_mutex_trylock(mutex);
            CHECK_EQUAL(AMP_SUCCESS, retval);
            {
                // Critical section. Nothing to do.
            }
            retval = amp_mutex_unlock(mutex);
            CHECK_EQUAL(AMP_SUCCESS, retval);
        }
        
        retval = amp_mutex_destroy(&mutex,
                                   AMP_DEFAULT_ALLOCATOR);
        CHECK_EQUAL(AMP_SUCCESS, retval);
    }
    
    
    // The following test will trigger an assert if NDEBUG isn't defined
    // and will result in undefined behavior if it is defined. Therefore
    // it makes no sense to run the test at all as long as the documented
    // behavior of amp_mutex_lock according to recursive locking doesn't change.
#if 0
    TEST(check_recursive_locking_not_allowed)
    {
        amp_mutex_t mutex;
        
        int retval = amp_mutex_create(&mutex,
                                      AMP_DEFAULT_ALLOCATOR,
                                      &amp_default_alloc,
                                      &amp_default_dealloc);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        {
            int const milliseconds = 50;
            UNITTEST_TIME_CONSTRAINT(milliseconds);
            
            retval = amp_mutex_lock(mutex);
            CHECK_EQUAL(AMP_SUCCESS, retval);
            {
                // Try to lock recursively - should return an error code.
                // If this deadlocks there is no way for the unit test to 
                // recover.
                int rv = amp_mutex_lock(mutex);
                CHECK_EQUAL(EDEADLK, rv);
                
                rv = amp_mutex_trylock(mutex);
                CHECK(EBUSY == rv);
                
            }
            retval = amp_mutex_unlock(mutex);
            CHECK_EQUAL(AMP_SUCCESS, retval);
        }
        
        retval = amp_mutex_destroy(mutex,
                                   AMP_DEFAULT_ALLOCATOR,
                                   &amp_default_dealloc);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
    }
#endif // 0
    
    
    namespace 
    {
        typedef int check_flag_t;
        
        check_flag_t const CHECK_FLAG_UNSET = 0;
        check_flag_t const CHECK_FLAG_SET = 775;
        
        
        struct mutex_and_one_check_flag_s {
            amp_mutex_t mutex;
            check_flag_t check_flag;
        };
        
        
        void unsuccessful_trylock_thread_func(void *ctxt)
        {
            struct mutex_and_one_check_flag_s *context = 
                static_cast<struct mutex_and_one_check_flag_s*>(ctxt);
            
            // Should be unable to get the lock.
            int retval = amp_mutex_trylock(context->mutex);
            if ((EBUSY == retval) || (EDEADLK == retval)) {
                context->check_flag = CHECK_FLAG_SET;
            }
        }
        
        
    } // anonymous namespace
    
    TEST(two_threads_one_locks_one_trylocks)
    {
        struct mutex_and_one_check_flag_s mutex_and_flag;
        mutex_and_flag.check_flag = CHECK_FLAG_UNSET;
        
        int retval = amp_mutex_create(&mutex_and_flag.mutex,
                                      AMP_DEFAULT_ALLOCATOR);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        retval = amp_mutex_lock(mutex_and_flag.mutex);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        amp_thread_t thread = AMP_THREAD_UNINITIALIZED;
        retval = amp_thread_create_and_launch(&thread,
                                              AMP_DEFAULT_ALLOCATOR,
                                              &mutex_and_flag, 
                                              &unsuccessful_trylock_thread_func);
        assert(AMP_SUCCESS == retval);
        
        retval = amp_thread_join_and_destroy(&thread,
                                             AMP_DEFAULT_ALLOCATOR);
        assert(AMP_SUCCESS == retval);
        
        CHECK_EQUAL(CHECK_FLAG_SET, mutex_and_flag.check_flag);
        
        
        retval = amp_mutex_unlock(mutex_and_flag.mutex);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        retval = amp_mutex_destroy(&mutex_and_flag.mutex,
                                   AMP_DEFAULT_ALLOCATOR);
        CHECK_EQUAL(AMP_SUCCESS, retval);
    }
    
    // The following test will trigger an assert if NDEBUG isn't defined
    // and will result in undefined behavior if it is defined. Therefore
    // it makes no sense to run the test at all as long as the documented
    // behavior of amp_mutex_unlock doesn't change.
#if 0
    namespace 
    {
        
        void bad_unlock_thread_func(void *ctxt)
        {
            struct mutex_and_one_check_flag_s *context = 
            static_cast<struct mutex_and_one_check_flag_s*>(ctxt);
            
            int retval = amp_mutex_unlock(context->mutex);
            if (EPERM == retval || AMP_SUCCESS != retval) {
                context->check_flag = CHECK_FLAG_SET;
            }

        }
        
    } // anonymous namespace
    
    
    TEST(wrong_thread_tries_to_unlock)
    {
        struct mutex_and_one_check_flag_s mutex_and_flag;
        mutex_and_flag.check_flag = CHECK_FLAG_UNSET;
        
        int retval = amp_mutex_create(&mutex_and_flag.mutex,
                                      AMP_DEFAULT_ALLOCATOR,
                                      &amp_default_alloc,
                                      &amp_default_dealloc);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        retval = amp_mutex_lock(mutex_and_flag.mutex);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        amp_thread_t thread = AMP_THREAD_UNINITIALIZED;
        retval = amp_thread_create_and_launch(&thread, 
                                              AMP_DEFAULT_ALLOCATOR,
                                              &amp_default_alloc,
                                              &amp_default_dealloc,
                                              &mutex_and_flag, 
                                              &bad_unlock_thread_func);
        assert(AMP_SUCCESS == retval);
        
        retval = amp_thread_join_and_destroy(thread,
                                             AMP_DEFAULT_ALLOCATOR,
                                             &amp_default_dealloc);
        assert(AMP_SUCCESS == retval);
        
        CHECK_EQUAL(CHECK_FLAG_SET, mutex_and_flag.check_flag);
        
        retval = amp_mutex_unlock(mutex_and_flag.mutex);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        retval = amp_mutex_destroy(mutex_and_flag.mutex,
                                   AMP_DEFAULT_ALLOCATOR,
                                   &amp_default_dealloc);
        CHECK_EQUAL(AMP_SUCCESS, retval);      
    } 
#endif // 0
    
    namespace 
    {
        
        struct staggered_data_s {
            amp_mutex_t *mutex_p;
            size_t *lock_counter_p;
            
            size_t count_at_lock;
            int return_code;
            check_flag_t check_flag;
        };
        
        // Locks the mutex and when it can enter the critical section
        // increments the protexted counter and stores the resulting count in
        // its context and sets the check flag.
        void staggered_locking_thread_func(void *ctxt)
        {
            struct staggered_data_s *context = 
                static_cast<struct staggered_data_s*>(ctxt);
            
            context->return_code = AMP_SUCCESS;
            
            int retval = amp_mutex_lock(*context->mutex_p);
            if (AMP_SUCCESS != retval) {
                context->return_code = retval;
                return;
            }
            {
                // Critical section.
                context->count_at_lock = ++(*(context->lock_counter_p));
            }
            retval = amp_mutex_unlock(*context->mutex_p);
            if (AMP_SUCCESS != retval) {
                context->return_code = retval;
                return;
            }
            
            context->check_flag = CHECK_FLAG_SET;
            
        }
        
        
    } // anonymous namespace
    
    
    
    TEST(staggered_locking)
    {
        // Create and lock a mutex. Launch a number of threads that all
        // lock and wait on the mutex.
        // Unlock the mutex and join with all threads.
        // Check that all threads successfully entered the mutex-protected
        // critical section and that every thread saw another counter.
        
        amp_mutex_t mutex;
        int retval = amp_mutex_create(&mutex,
                                      AMP_DEFAULT_ALLOCATOR);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        retval = amp_mutex_lock(mutex);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        size_t const thread_count = 20;
        struct staggered_data_s thread_contexts[thread_count];
        
        size_t lock_counter = 0;
        
        
        amp_thread_array_t threads = AMP_THREAD_ARRAY_UNINITIALIZED;
        retval = amp_thread_array_create(&threads,
                                         AMP_DEFAULT_ALLOCATOR,
                                         thread_count);
        assert(AMP_SUCCESS == retval);
        
        for (size_t i = 0; i < thread_count; ++i) {
            
            thread_contexts[i].mutex_p = &mutex;
            thread_contexts[i].lock_counter_p = &lock_counter;
            thread_contexts[i].count_at_lock = 0;
            thread_contexts[i].return_code = AMP_SUCCESS;
            thread_contexts[i].check_flag = CHECK_FLAG_UNSET;
            
            int const retv = amp_thread_array_configure(threads,
                                                        i, 
                                                        1,
                                                        &thread_contexts[i],
                                                        staggered_locking_thread_func);
            assert(AMP_SUCCESS == retv);
            (void)retv;
            
        }
        
        size_t joinable_count = 0;
        retval = amp_thread_array_launch_all(threads,
                                             &joinable_count);
        assert(AMP_SUCCESS == retval);
        assert(thread_count == joinable_count);
        
        
        // Unlock the mutex to allow staggered access to the critical section
        // by the threads (that might already been blocked while trying to lock
        // it).
        retval = amp_mutex_unlock(mutex);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        

        // Join with all threads - which means that they all locked and unlocked
        // the mutex and worked in the critial section if no error occured.
        retval = amp_thread_array_join_all(threads,
                                           &joinable_count);
        assert(AMP_SUCCESS == retval);
        assert(0 == joinable_count);
        
        retval = amp_thread_array_destroy(&threads,
                                          AMP_DEFAULT_ALLOCATOR);
        assert(AMP_SUCCESS == retval);
        
        
        // Check that all threads increased the lock counter.
        CHECK_EQUAL(lock_counter, thread_count);
        
        // Arrays that associate element with lock counts of the threads and
        // is then checked that every element is set (index zero isn't set
        // as the thread funcs first increase the counter and then store it).
        check_flag_t check_count_at_lock_array[thread_count + 1];
        for (size_t i = 0; i < thread_count + 1; ++i) {
            check_count_at_lock_array[i] = CHECK_FLAG_UNSET;
        }
    
        
        // Check that valid values are set in the contexts.
        for (size_t i = 0; i < thread_count; ++i) {
            CHECK(0 != thread_contexts[i].count_at_lock);
            CHECK_EQUAL(AMP_SUCCESS, thread_contexts[i].return_code);
            CHECK_EQUAL(CHECK_FLAG_SET, thread_contexts[i].check_flag);
            if (CHECK_FLAG_SET == thread_contexts[i].check_flag) {
                size_t index = thread_contexts[i].count_at_lock;
                check_count_at_lock_array[index] = CHECK_FLAG_SET;
            }
        }
        
        // All (other than the first) indexes set?
        for (size_t i = 1; i < thread_count + 1; ++i) {
            CHECK_EQUAL(CHECK_FLAG_SET, check_count_at_lock_array[i]);
        }
        
        
        retval = amp_mutex_destroy(&mutex,
                                   AMP_DEFAULT_ALLOCATOR);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
    }
    
    
    
} // SUITE(amp_mutex)


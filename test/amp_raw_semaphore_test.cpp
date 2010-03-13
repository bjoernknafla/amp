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
 * Unit tests for amp_raw_semaphore.
 *
 * TODO: @todo Add time constraints for tests that mustn't block.
 */

#include <UnitTest++.h>


#include <amp/amp_raw_semaphore.h>
#include <amp/amp_raw_thread.h>
#include <amp/amp_stddef.h>

SUITE(amp_raw_semaphore)
{
    TEST(wait_on_init_signaled_semaphore)
    {
        // Create a signaled semaphore and let the function wait on it and then
        // check the return value of the wait call for AMP_SUCCESS.
        // If the semaphore is malfuncitoning this will block the whole unit
        // tests...
        
        
        struct amp_raw_semaphore_s sem;
        int retval = amp_raw_semaphore_init(&sem, 1);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        {
            int const milliseconds = 50;
            UNITTEST_TIME_CONSTRAINT(milliseconds);
            
            retval = amp_raw_semaphore_wait(&sem);
            CHECK_EQUAL(AMP_SUCCESS, retval);
        }
        
        retval = amp_raw_semaphore_finalize(&sem);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
    }
    
    
    
    TEST(wait_on_init_and_then_signaled_semaphore)
    {
        // Create a non-signaled semaphore, then signal it and let the function 
        // wait on it and then check the return value of the wait call for 
        // AMP_SUCCESS.
        // If the semaphore is malfuncitoning this will block the whole unit
        // tests...
        
        
        struct amp_raw_semaphore_s sem;
        int retval = amp_raw_semaphore_init(&sem, 0);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        retval = amp_raw_semaphore_signal(&sem);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        {
            int const milliseconds = 50;
            UNITTEST_TIME_CONSTRAINT(milliseconds);
        
            retval = amp_raw_semaphore_wait(&sem);
            CHECK_EQUAL(AMP_SUCCESS, retval);
        }
        
        retval = amp_raw_semaphore_finalize(&sem);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
    }
    
    
    
    namespace 
    {
        int const CHECK_FLAG_UNSET = 0;
        int const CHECK_FLAG_SET = 77;
        
        struct semaphore_flag_s {
            struct amp_raw_semaphore_s sem;
            int check_flag;
        };
        
        
        void wait_on_semaphore_and_set_flag_func(void *context)
        {
            struct semaphore_flag_s *sem_flag = static_cast<struct semaphore_flag_s*>(context);
            
            amp_raw_semaphore_wait(&(sem_flag->sem));
            sem_flag->check_flag = CHECK_FLAG_SET;
        }
        
    } // anonymous namespace
    
    
    TEST(thread_wait_on_init_signaled_semaphore)
    {
        // Create a signaled semaphore and let a thread wait on it and then set
        // a check flag. Join with the thread and check the flag.
        
        struct semaphore_flag_s sem_flag;
        int retval = amp_raw_semaphore_init(&(sem_flag.sem), 1);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        sem_flag.check_flag = CHECK_FLAG_UNSET;

        
        struct amp_raw_thread_s thread;
        retval = amp_raw_thread_launch(&thread, 
                                        &sem_flag, 
                                        wait_on_semaphore_and_set_flag_func);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        // Joins after the thread waited and passed the semaphore.
        retval = amp_raw_thread_join(&thread);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK_EQUAL(CHECK_FLAG_SET, sem_flag.check_flag);
        
        retval = amp_raw_semaphore_finalize(&sem_flag.sem);
        CHECK_EQUAL(AMP_SUCCESS, retval);
    }
    
    
    
    TEST(thread_wait_on_init_then_signal_semaphore)
    {
        struct semaphore_flag_s sem_flag;
        int retval = amp_raw_semaphore_init(&(sem_flag.sem), 0);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        sem_flag.check_flag = CHECK_FLAG_UNSET;
        
        
        struct amp_raw_thread_s thread;
        retval = amp_raw_thread_launch(&thread, 
                                        &sem_flag, 
                                        wait_on_semaphore_and_set_flag_func);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        // TODO: @todo Add a sleep here increase the possibility for the 
        //             thread to actually waiting on the semaphore.
        
        retval = amp_raw_semaphore_signal(&(sem_flag.sem));
        
        // Joins after the thread waited and passed the semaphore.
        retval = amp_raw_thread_join(&thread);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        CHECK_EQUAL(CHECK_FLAG_SET, sem_flag.check_flag);
        
        retval = amp_raw_semaphore_finalize(&sem_flag.sem);
        CHECK_EQUAL(AMP_SUCCESS, retval);    
    }
    
    
    
    namespace
    {
        struct some_threads_wait_one_signals_s {
            struct amp_raw_semaphore_s *sem_p;
            int check_flag;
        };
        
        void thread_to_wait_func(void *context)
        {
            struct some_threads_wait_one_signals_s *data = static_cast<struct some_threads_wait_one_signals_s*>(context);
            
            amp_raw_semaphore_wait(data->sem_p);
            data->check_flag = CHECK_FLAG_SET;
            
            // Let the next thread through, too.
            amp_raw_semaphore_signal(data->sem_p);
        }
        
    } // anonymous namespace
    
    TEST(some_threads_wait_one_signals)
    {
        // Create threads, one runs a function that won't wait on the semaphore 
        // but set a check variable and then signals the semaphore. The waiting
        // threads now pass one by one and store the check variable in an array
        // that is then tested to contain the correct value per element.
        
        struct amp_raw_semaphore_s sem;
        int retval = amp_raw_semaphore_init(&sem, 0);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        std::size_t const threads_to_wait_count = 4;
        
        // Set up access to semaphore and check flags to set by waiting threads.
        struct some_threads_wait_one_signals_s checks[threads_to_wait_count];
        for (std::size_t i = 0; i < threads_to_wait_count; ++i) {
            checks[i].sem_p = &sem;
            checks[i].check_flag = CHECK_FLAG_UNSET;
        }
        
        // Launch waiting threads.
        struct amp_raw_thread_s threads_to_wait[threads_to_wait_count];
        for (std::size_t i = 0; i < threads_to_wait_count; ++i) {
            retval = amp_raw_thread_launch(&threads_to_wait[i], 
                                            &checks[i],
                                            thread_to_wait_func);
            CHECK_EQUAL(AMP_SUCCESS, retval);
        }
        
        
        // TODO: @todo Decide if to loop here a few times to give the threads
        //             a chance to start and reach the non-signaled semaphore.
        
        // Check that no thread has yet passed the semaphoreand therefore
        // that all check flags are set to CHECK_FLAG_UNSET. This is no
        // absolute test for correctness as the threads might
        // not even have started running yet, but bettern than nothing.
        for (std::size_t i = 0; i < threads_to_wait_count; ++i) {
            CHECK_EQUAL(CHECK_FLAG_UNSET, checks[i].check_flag);
        }
        
        // Signal semaphore.
        retval = amp_raw_semaphore_signal(&sem);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        // Join all threads - which means they waited and passed the semaphore.
        for (std::size_t i = 0; i < threads_to_wait_count; ++i) {
            retval = amp_raw_thread_join(&threads_to_wait[i]);
            CHECK_EQUAL(AMP_SUCCESS, retval);
        }
        
        // Check value of check flags.
        for (std::size_t i = 0; i < threads_to_wait_count; ++i) {
            CHECK_EQUAL(CHECK_FLAG_SET, checks[i].check_flag);
        }
        
        retval = amp_raw_semaphore_finalize(&sem);
        CHECK_EQUAL(AMP_SUCCESS, retval);
    }
    
} // SUITE(amp_raw_semaphore)

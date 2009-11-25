/*
 *  amp_thread_group_test.cpp
 *  amp
 *
 *  Created by Björn Knafla on 18.11.09.
 *  Copyright 2009 Bjoern Knafla Parallelization + AI + Gamedev Consulting. All rights reserved.
 *
 */

#include <UnitTest++.h>

#include <vector>

#include <stddef.h>
#include <assert.h>
#include <errno.h>

#include <amp/amp_stddef.h>
#include <amp/amp_raw_thread.h>
#include <amp/amp_byte_range.h>



SUITE(amp_thread_group)
{
    namespace {
        
        
        
        /*
        typedef int (*amp_alloc_func)(void *allocator_context, 
                                      void **pointer, 
                                      size_t bytes_to_allocate);
        
        typedef int (*amp_dealloc_func)(void *allocator_context,
                                       void *pointer);
        
                     
        int amp_malloc(void *allocator_context,
                       void **pointer,
                       size_t bytes_to_allocate)
        {
            (void)allocator_context;
            
            assert(NULL != pointer);

            if (NULL == pointer) {
                return EINVAL;
            }
            
            *pointer = malloc(bytes_to_allocate);
            
            if (NULL == *pointer) {
                return errno;
            }
            
            return AMP_SUCCESS;
        }
                     
                     
        
        int amp_free(void *allocator_context,
                     void *pointer)
        {
            (void)allocator_context;
            
            free(pointer);
            
            return AMP_SUCCESS;
        }
        */
        
        
        typedef void* (*amp_alloc_func)(void *allocator_context, 
                                        size_t bytes_to_allocate);
        
        typedef void (*amp_dealloc_func)(void *allocator_context,
                                         void *pointer);
        
        
         
         void* amp_malloc(void *dummy_allocator_context, 
                          size_t bytes_to_allocate)
         {
             (void)dummy_allocator_context;
         
             return malloc(bytes_to_allocate);
         }
         
         
         void amp_free(void *dummy_allocator_context,
                       void *pointer)
         {
         (void)dummy_allocator_context;
         
         free(pointer);
         }
         
         
        
        struct amp_thread_group_context_s {
            amp_alloc_func alloc;
            amp_dealloc_func dealloc;
            void *allocator_context;
        };
        
        
        struct amp_thread_group_thread_s {
            struct amp_raw_thread_s thread;
            void *thread_context;
            amp_raw_thread_func_t thread_func;
        };
        
        
        struct amp_thread_group_s {
            struct amp_thread_group_thread_s *threads;
            size_t thread_count;
            size_t joinable_count;
            struct amp_thread_group_context_s *context;
        };
        
        
        /**
         *
         * Doesn't take over ownership of group_context, thread_contexts, or
         * thread_functions. However, the group context and the items inside
         * thread_contexts and thread_functions (but not the ranges itself) are 
         * used throughout the lifetime of the thread group and mustn't be
         * destroyed until the thread group has been destroyed.
         * 
         *
         * thread_contexts and thread_functions ranges must be at least 
         * thread_count times advanceable.
         *
         * TODO: @todo Add error handling.
         * TODO: @todo Decide if to allow @c 0 as a thread count.
         */
        int amp_thread_group_create(struct amp_thread_group_s **thread_group,
                                    struct amp_thread_group_context_s *group_context,
                                    size_t thread_count,
                                    struct amp_byte_range_s const *thread_contexts,
                                    amp_raw_thread_func_t thread_function)
        {
            assert(NULL != thread_group);
            assert(NULL != group_context);
            assert(NULL != group_context->alloc);
            assert(NULL != group_context->dealloc);
            assert(NULL != thread_contexts);
            assert(NULL != thread_function);
            assert(thread_count <= amp_byte_range_advanceable_count(thread_contexts));
            
            if (   (NULL == thread_group) 
                || (NULL == group_context) 
                || (NULL == thread_contexts) 
                || (NULL == thread_function)) {

                return EINVAL;
            }
            
            if (thread_count > amp_byte_range_advanceable_count(thread_contexts)) {
                return EINVAL;
            }
            
            *thread_group = NULL;
            
            struct amp_thread_group_s *group = 
                (struct amp_thread_group_s *)group_context->alloc(group_context->allocator_context,
                                                                  sizeof(struct amp_thread_group_s));
            assert(NULL != group);
            if (NULL == group) {
                return ENOMEM;
            }
            
            /* Allocate memory for the groups threads. */
            struct amp_thread_group_thread_s* threads = 
                (struct amp_thread_group_thread_s*)group_context->alloc(group_context->allocator_context,
                                                                        sizeof(struct amp_thread_group_thread_s) * thread_count);
            assert(NULL != threads);
            if (NULL == threads) {
                
                group->context->dealloc(group_context->allocator_context,
                                        group);
                
                return ENOMEM;
            }
            
            /* Set the groups threads contexts and functions. */
            struct amp_byte_range_s tctxts;
            
            amp_byte_range_copy_from_to(thread_contexts, &tctxts);
            
            for (size_t i = 0; i < thread_count; ++i) {
                /* These asserts shouldn't ever trigger as the ranges are 
                 * already checked before.
                 */
                assert(AMP_FALSE == amp_byte_range_is_empty(&tctxts));
                
                threads[i].thread_context = amp_byte_range_get_front(&tctxts);
                threads[i].thread_func = thread_function;
                amp_byte_range_advance_front(&tctxts);
            }
            
            group->threads = threads;
            group->thread_count = thread_count;
            group->joinable_count = 0;
            group->context = group_context;
            
            *thread_group = group;
            
            return AMP_SUCCESS;
        }
        
        /**
         *
         * If any thread groups threads have been launched they must be 
         * joined before destroying the group. It isn't done automatically
         * because joining can block and the amp user must know what she is
         * doing and explicitly decide how to handle shutdown.
         */
        int amp_thread_group_destroy(struct amp_thread_group_s *thread_group)
        {
            assert(NULL != thread_group);
            assert(0 == thread_group->joinable_count);
            
            if (NULL == thread_group) {
                return EINVAL;
            }
            
            if ((0 != thread_group->joinable_count)) {
                return EBUSY;
            }
            
            thread_group->context->dealloc(thread_group->context->allocator_context,
                                                 thread_group->threads);

            thread_group->context->dealloc(thread_group->context->allocator_context,
                                                 thread_group);
            
            return AMP_SUCCESS;
        }
        
        
        /**
         * 
         * Launches the contained threads one after the other and stops if
         * thread launching fails. The number of threads launched is returned
         * in joinable if the pointer is not NULL.
         * If not all threads could be launched nothing is done other then
         * returning EAGAIN or ENOMEM and the threads that could be launched
         * are running and not joined with. 
         * Call join all to join with the launched threads after the thread
         * functions have exited.
         *
         * You can handle non-launcheable threads of a thread group in at least
         * three ways:
         * <ol>
         *    <li> Terminate the app if the number of launched threads doesn't
         *         equal the number of threads the thread group should have 
         *         launched. Call join all on the thread group if you want to 
         *         end the application in a coordinated way.
         *    </li>
         *    <li> Order the thread contexts and functions so that even if not 
         *         all threads of the group could be launched the launched
         *         threads are able to work with each other and don't need the
         *         non-launched thread (functions).
         *    </li>
         *    <li> Re-run launch all as long until joinable indicates that
         *         all threads of the thread group have been launched and
         *         can therefore be joined.
         *         Be careful with join all counts between launch all calls as
         *         launch all will try to launch non-launched and already joined 
         *         threads.
         *    </li>
         * </ol>
         * In both cases it makes sense to code the thread functions so the
         * launch process runs through two stages (launch threads into "orbit"
         * and after all necessary thread functions signalled that they are in 
         * orbit let them start their work). Each thread function uses a 
         * condition variable to signal that it wants to proceed after it has
         * been launched. The condition variable predicate can then signal 
         * that the function should proceed or that it should shut down, e.g. if
         * not all necessary threads could have been launched.
         */
        int amp_thread_group_launch_all(struct amp_thread_group_s *thread_group,
                                        size_t *joinable)
        {
            assert(NULL != thread_group);
            
            if (NULL == thread_group) {
                return EINVAL;
            }
            
            size_t joinable_count = thread_group->joinable_count;
            size_t const thread_count = thread_group->thread_count;
            
            int retval = AMP_SUCCESS;
            while (   (joinable_count < thread_count)
                   && (AMP_SUCCESS == retval)) {
                
                retval = amp_raw_thread_launch(&(thread_group->threads[joinable_count].thread), 
                                               thread_group->threads[joinable_count].thread_context, 
                                               thread_group->threads[joinable_count].thread_func);
                
                if (AMP_SUCCESS == retval) {
                    ++joinable_count;
                }
            }
            
            thread_group->joinable_count = joinable_count;
            
            if (NULL != joinable) {
                *joinable = joinable_count;
            }
            
            return retval;
        }
        
        
        int amp_thread_group_join_all(struct amp_thread_group_s *thread_group,
                                      size_t *joined)
        {
            assert(NULL != thread_group);
            
            if (NULL == thread_group) {
                return EINVAL;
            }
            
            
            size_t const joinable_count = thread_group->joinable_count;
            size_t joined_count = 0;
            int retval = AMP_SUCCESS;
            while (   (joined_count < joinable_count)
                   && (AMP_SUCCESS == retval)) {
             
                /* Launching from left to right, joining from right to left. */
                retval = amp_raw_thread_join(&(thread_group->threads[joinable_count - 1 - joined_count].thread));
                
                if (AMP_SUCCESS == retval) {
                    ++joined_count;    
                }
            }
            
            thread_group->joinable_count -= joined_count;
            
            
            if (NULL != joined) {
                *joined = joined_count;
            }
            
            return retval;
        }
        
        
        int amp_thread_group_get_context(struct amp_thread_group_s *thread_group, 
                                         struct amp_thread_group_context_s **context)
        {
            assert(NULL != thread_group);
            assert(NULL != context);
            
            if (NULL == thread_group || NULL == context) {
                return EINVAL;
            }
            
            *context = thread_group->context;
            
            return AMP_SUCCESS;
        }
        
        

        
        
        
        int const fortytwo = 42;
        
        void set_int_context_to_fortytwo(void *ctxt)
        {
            int *item = (int *)ctxt;
            *item = 42;
        }
        
        
    } // anonymous namespace
    
    TEST(create_destroy_amp_thread_group)
    {
        
        struct amp_thread_group_s *thread_group;
        
        struct amp_thread_group_context_s thread_group_context;
        thread_group_context.alloc = amp_malloc;
        thread_group_context.dealloc = amp_free;
        thread_group_context.allocator_context = NULL;
        
        size_t const thread_count = 16;
        std::vector<int> context_vector(thread_count, 0);
        
        struct amp_byte_range_s context_stream;
        int retval = amp_byte_range_init_with_item_count(&context_stream,
                                                         &context_vector[0],
                                                         thread_count,
                                                         sizeof(std::vector<int>::value_type));
        assert(AMP_SUCCESS == retval);
    
        retval = amp_thread_group_create(&thread_group,
                                         &thread_group_context,
                                         thread_count,
                                         &context_stream,
                                         set_int_context_to_fortytwo);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        size_t number_of_joinable_threads_after_launch_all = 0;
        retval = amp_thread_group_launch_all(thread_group, &number_of_joinable_threads_after_launch_all);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        CHECK_EQUAL(thread_count, number_of_joinable_threads_after_launch_all);
        
        size_t number_of_joined_threads_after_join_all = 0;
        retval = amp_thread_group_join_all(thread_group, &number_of_joined_threads_after_join_all);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        CHECK_EQUAL(number_of_joinable_threads_after_launch_all, number_of_joined_threads_after_join_all);
        
        struct amp_thread_group_context_s *ctxt = NULL;
        
        retval = amp_thread_group_get_context(thread_group, &ctxt);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        CHECK_EQUAL(ctxt, &thread_group_context);
        
        
        retval = amp_thread_group_destroy(thread_group);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        for (size_t i = 0; i < thread_count; ++i) {
            CHECK_EQUAL(fortytwo, context_vector[i]); 
        }
        
    }
    
    
    
} // SUITE(amp_thread_group)



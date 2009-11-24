/*
 *  amp_thread_group_test.cpp
 *  amp
 *
 *  Created by Björn Knafla on 18.11.09.
 *  Copyright 2009 Bjoern Knafla Parallelization + AI + Gamedev Consulting. All rights reserved.
 *
 */

#include <UnitTest++.h>

#include <stddef.h>

#include <amp/amp_stddef.h>
#include <amp/amp_raw_thread.h>
#include <amp/amp_byte_range.h>



SUITE(amp_thread_group)
{
    namespace {
        
        

        
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
        
        
        /*
        typedef void* (*amp_alloc_func)(void *allocator_context, 
                                        size_t bytes_to_allocate);
        
        typedef void (*amp_dealloc_func)(void *allocator_context,
                                         void *pointer);
        
        
         
         void* amp_malloc(void *dummy_allocator_context, 
         size_t bytes_to_allocate)
         {
         (void)allocator_context;
         
         return malloc(bytes_to_allocate);
         }
         
         
         void amp_free(void *dummy_allocator_context,
         void *pointer)
         {
         (void)dummy_allocator_context;
         
         free(pointer);
         }
         
         */
         
         
        
        struct amp_thread_group_context_s {
            amp_alloc_func alloc;
            amp_dealloc_func dealloc;
            void *allocator_context;
        };
        
        
        struct amp_thread_group_s {
          
            struct amp_thread_group_context_s *context;
        };
        
        
        int amp_thread_group_create(&thread_group,
                                    &thread_group_context,
                                    thread_count,
                                    &context_stream,
                                    &func_stream)
        {
            
        }
        
        
        int amp_thread_group_destroy(&thread_group)
        {
            
        }
        
        
        
        int amp_thread_group_launch_all(&thread_group)
        {
            
        }
        
        
        int amp_thread_group_join_all(&thread_group)
        {
            
        }
        
        
        int amp_thread_group_get_context(&thread_group, &ctxt)
        {
            
        }
        
        
        int amp_thread_group_get_streams(&thread_group,
                                     &c_stream,
                                     &f_stream)
        {
            
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
        
        struct amp_thread_group_t thread_group;
        
        struct amp_thread_group_context_s thread_group_context;
        thread_group_context.alloc = amp_malloc;
        thread_group_context.dealloc = amp_free;
        thread_group_context.allocator_context = NULL;
        
        size_t const thread_count = 16;
        std::vector<int> context_vector(thread_count, 0);
        
        struct amp_byte_range_s context_stream;
        int retval = amp_byte_range_init_with_item_count(&contet_stream,
                                                         &context_vector[0],
                                                         thread_count,
                                                         sizeof(typename context_vector::value_type));
        assert(AMP_SUCCESS == retval);
        
        struct amp_byte_range_s func_stream;
        retval = amp_byte_range_init(&func_stream,
                                     &set_int_context_to_fortytwo,
                                     &set_int_context_to_fortytwo + 1,
                                     0,
                                     sizeof(set_int_context_to_fortytwo));
        assert(AMP_SUCCESS == retval);

        
        retval = amp_thread_group_create(&thread_group,
                                         &thread_group_context,
                                         thread_count,
                                         &context_stream,
                                         &func_stream);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        retval = amp_thread_group_launch_all(&thread_group);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        retval = amp_thread_group_join_all(&thread_group);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        struct amp_byte_range_s *c_stream = NULL;
        struct amp_byte_range_s *f_stream = NULL;
        struct amp_thread_group_context_s *ctxt = NULL;
        
        retval = amp_thread_group_get_context(&thread_group, &ctxt);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        CHECK_EQUAL(ctxt, &thread_group_context);
        
        retval = amp_thread_group_get_streams(&thread_group,
                                              &c_stream,
                                              &f_stream);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        CHECK_EQUAL(c_stream, &context_stream);
        CHECK_EQUAL(f_stream, &func_stream);
        
        retval = amp_thread_group_destroy(&thread_group);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        for (size_t i = 0; i < thread_count; ++i) {
            CHECK_EQUAL(fortytwo, context_vector[i]); 
        }
        
    }
    
    
    
} // SUITE(amp_thread_group)



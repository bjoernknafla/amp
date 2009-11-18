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

#include <amp/amp_raw_thread.h>



SUITE(amp_thread_group)
{
    namespace {
        
        
        struct amp_raw_data_stream {
            void *data;
            size_t item_type_size_in_bytes;
            size_t offset_in_bytes;
            size_t stride_in_bytes;
            size_t item_iteration_stride;
            size_t item_count;
            size_t stream_size_in_bytes;
        };
        
#error Why use offsets?????
        
        
        AMP_BOOL amp_raw_data_stream_is_valid(struct amp_raw_data_stream *stream)
        {
            if (NULL == stream) {
                return AMP_FALSE;
            }
            
            size_t item_size = stream->item_type_size_in_bytes;
            size_t offset = stream->offset_in_bytes;
            size_t stride = stream->stride_in_bytes;
            size_t item_iter_stride = stream->item_iteration_stride;
            size_t item_count = stream->item_count;
            size_t stream_size = stream->stream_size_in_bytes;
            
            AMP_BYTE *byte_stream_begin = (AMP_BYTE *)stream->data;
            
            AMP_BYTE *byte_item_end = byte_stream_begin + offset + ((item_count - 1) * item_iter_stride * stride) + item_size;
            
            if (byte_item_end > (byte_stream_begin + stream_size_in_bytes)) {
                return AMP_FALSE;
            }
            
            return AMP_TRUE;
        }
        
        int amp_raw_data_stream_get_item(struct amp_raw_data_stream *stream,
                                         size_t item_index,
                                         void **item)
        {
            assert(amp_raw_data_stream_is_valid(stream));
            assert(NULL != item);
            assert(item_index < stream->item_count);
            
            AMP_BYTE *byte_stream_begin = (AMP_BYTE *)stream.data;
            
            AMP_BYTE *bytes = byte_stream_begin + offset_in_bytes + (item_index * item_iteration_stride * stride_in_bytes );
            
            assert(bytes < (((AMP_BYTE *)stream->data) + stream_size_in_bytes));
            
            *item = bytes;
        }
        
        
        
        typedef void* (*amp_alloc_func)(void *allocator_context, 
                                        size_t bytes_to_allocate);
        
        typedef void (*amp_dealloc_func)(void *allocator_context,
                                         void *pointer);
        
        
        
        struct amp_thread_group_context_s {
            amp_alloc_func alloc;
            amp_dealloc_func dealloc;
            void *allocator_context;
        };
        
        
        
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
        
        
        int const fortytwo = 42;
        
        void set_int_context_to_fortytwo(void *ctxt)
        {
            int *item = (int *)ctxt;
            *item = 42;
        }
        
        
    } // anonymous namespace
    
    TEST(create_destroy_amp_thread_group)
    {
        
        amp_thread_group_t thread_group;
        
        struct amp_thread_group_context_s thread_group_context;
        thread_group_context.alloc = amp_malloc;
        thread_group_context.dealloc = amp_free;
        thread_group_context.allocator_context = NULL;
        
        size_t const thread_count = 16;
        std::vector<int> context_vector(thread_count, 0);
        
        struct amp_raw_data_stream_s context_stream;
        context_stream.items = &context_vector[0];
        context_stream.item_type_size_in_bytes =sizeof(typename context_vector::value_type);
        context_stream.items_offset = 0;
        context_stream.items_stride = 1;
        context_stream.item_count = thread_count;
        
        struct amp_raw_data_stream_s func_stream;
        func_stream.items = set_int_context_to_fortytwo;
        func_stream.item_type_size_in_bytes = sizeof(dummy_func);
        func_stream.items_offset = 0;
        func_stream.items_stride = 0; // Means, only the first func will be used
        func_stream.item_count = 1;
        
        int retval = amp_thread_group_create(&thread_group,
                                             &thread_group_context,
                                             thread_count,
                                             &context_stream,
                                             &func_stream);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        retval = amp_thread_group_launch_all(&thread_group);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        retval = amp_thread_group_join_all(&thread_group);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        
        struct amp_raw_data_stream_s *c_stream = NULL;
        struct amp_raw_data_stream_s *f_stream = NULL;
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



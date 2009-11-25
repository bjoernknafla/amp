/*
 *  amp_byte_range.h
 *  amp
 *
 *  Created by Björn Knafla on 24.11.09.
 *  Copyright 2009 Bjoern Knafla Parallelization + AI + Gamedev Consulting. All rights reserved.
 *
 */

/**
 * Description of a range of items inside a byte stream that can be 
 * iterated over once from the beginning to the end of the range.
 *
 * The range only sees bytes and its only knowledge of the items in the byte
 * stream are defined by the user as the item type size in bytes and the 
 * distance in bytes from the beginning of one item to the next.
 *
 * A range itself doesn't alter or modify the data stream it points to but a
 * range user can modify the elements of the stream accessed via 
 * amp_byte_stream_get_front.
 *
 * Mostly a helper structure for amp_thread_group.
 *
 * See Andrei Alexandrescu's slides from Boostcon 2009, "Iterators Must Go", http://www.boostcon.com/site-media/var/sphene/sphwiki/attachment/2009/05/08/iterators-must-go.pdf
 *
 * TODO: @todo Add a way to chain or merge ranges.
 * TODO: @todo Add a way to zip tuples of ranges.
 * TODO: @todo Add an item count function to the range interface.
 * TODO: @todo Add a way to split ranges to enable parallel work on range parts.
 * TODO: @todo Decide if a step size of 0 on init should check that at most one
 *             item is contained in the range?
 * TODO: @todo Add a unit test for the range copy function.
 */

#ifndef AMP_amp_byte_range_H
#define AMP_amp_byte_range_H


#if defined(__cplusplus)
extern "C" {
#endif

    
    
#include <stddef.h>
    
#include <amp/amp_stddef.h>
    
    
    
    /**
     * A range over a stream of bytes that contains items of a certain data type
     * which might be interleaved with other data and that can be emtpy and if 
     * it isn't empty allows access to its first item and advancing to the next 
     * item until the range becomes empty.
     *
     * Handle as an opaque data type and only use the access functions.
     */
    struct amp_byte_range_s {
        /* Current iterator position inside the range.
         */
        AMP_BYTE *begin;
        
        size_t item_type_size;
        
        /* Contains the item type size and the gap / stride to the next item.
         * If set to 0 an iterator advance won't move as if the range only 
         * contains one item.
         */
        size_t step_size_to_next_item;
        
        /* end is at least one item_type_size step behind the last contained 
         * item. 
         */
        AMP_BYTE *end;
    };
    
    
    
    /**
     * Initilaizes a range so its front pointer points to range_begin and which
     * contains items of byte size item_type_size and the beginning of an item
     * is step_size_from_item_to_item bytes away from the next item.
     *
     * range_begin points to the first item in the byte range.
     * range_end points behind the last byte of the range.
     *
     * step_size_from_item_to_item is the byte count to step from the 
     * beginning of the first byte of an item to the beginning
     * of the next item. It includes the item size and the byte gap from 
     * the end of the item to the beginning of the next.
     *
     * Returns @c AMP_SUCCESS on successful initialization.
     *         Passing NULL as range or an invalid configuration into the
     *         functions is an error that is asserted and might return EINVAL.
     */
    int amp_byte_range_init(struct amp_byte_range_s *range,
                            void *range_begin,
                            void *range_end,
                            size_t step_size_from_item_to_item,
                            size_t item_type_size);
    
    
    
    /**
     * Initializes a range so its front points to range_begin and the range
     * is configured to contain item_count items of byte size item_type_size
     * that are tightly packed in the data stream represented by the range.
     *
     * Returns @c AMP_SUCCESS on successful initialization.
     *         Passing NULL as range or an invalid configuration into the
     *         functions is an error that is asserted and might return EINVAL.
     */
    int amp_byte_range_init_with_item_count(struct amp_byte_range_s *range,
                                            void *range_begin,
                                            size_t item_count,
                                            size_t item_type_size);
    
    
    /**
     * Copies the configuration from source to target.
     *
     * source and target mustn't point to invalid ranges and mustn't be NULL.
     *
     * TODO: @todo Write a unit test for amp_byte_range_copy_from_to.
     */
    void amp_byte_range_copy_from_to(struct amp_byte_range_s const *source,
                                    struct amp_byte_range_s *target);
    
    
    /**
     * Returns @c AMP_TRUE if the range doesn't contain accessible data items,
     * otherwise returns @c AMP_FALSE.
     *
     * @attention range mustn't be NULL, otherwise behavior is undefined.
     */
    AMP_BOOL amp_byte_range_is_empty(struct amp_byte_range_s const *range);
    
    
    /**
     * Returns a pointer to the first byte of the data item at the front of the
     * range if the range isn't empty.
     *
     * void* is returned instead of AMP_BYTE* to prevent accidential pointer
     * arithmetic and force explicit casting by the caller to user the returned
     * data.
     *
     * @attention Behavior is undefined if range is empty.
     *
     * @attention range mustn't be NULL, otherwise behavior is undefined.
     */
    void* amp_byte_range_get_front(struct amp_byte_range_s const *range);
    
    
    /**
     * Advances the range to the next item after the front item if the range
     * isn't empty or if no next item exists results in an empty range.
     *
     * @attention Behavior is undefined if range is empty.
     *
     * @attention range mustn't be NULL, otherwise behavior is undefined.
     */
    void amp_byte_range_advance_front(struct amp_byte_range_s *range);
    
    
    
    /**
     * Returns the number of times the front of the range can be advanced until
     * it runs empty.
     * If a range can pop infinite elements (it is initialized with an 
     * step_size_from_item_to_item of @c 0) then it returns SIZE_MAX. To
     * find out if it really produces infinite return 
     */
    size_t amp_byte_range_advanceable_count(struct amp_byte_range_s const *range);
    
    
    
    /**
     * Returns AMP_TRUE if the range can't be emptied by advancing the front,
     * otherwise AMP_FALSE is returned.
     */
    AMP_BOOL amp_byte_range_is_infinitely_advanceable(struct amp_byte_range_s const *range);
    
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif
    

#endif /* AMP_amp_byte_range_H */

/*
 *  amp_byte_range.c
 *  amp
 *
 *  Created by Björn Knafla on 24.11.09.
 *  Copyright 2009 Bjoern Knafla Parallelization + AI + Gamedev Consulting. All rights reserved.
 *
 */

#include "amp_byte_range.h"

#include <assert.h>
#include <errno.h>




AMP_BOOL amp_byte_range_is_empty(struct amp_byte_range_s *range)
{
    assert(NULL != range);
    
    return (range->begin + range->item_type_size) > range->end;
}



void* amp_byte_range_get_front(struct amp_byte_range_s *range)
{
    assert(NULL != range);
    assert(AMP_FALSE == amp_byte_range_is_empty(range));
    
    return range->begin;
}



void amp_byte_range_advance_front(struct amp_byte_range_s *range)
{
    assert(NULL != range);
    assert(AMP_FALSE == amp_byte_range_is_empty(range));
    
    range->begin += range->step_size_to_next_item;
}




int amp_byte_range_init(struct amp_byte_range_s *range,
                        void *range_begin,
                        void *range_end,
                        size_t step_size_from_item_to_item,
                        size_t item_type_size)
{
    assert(NULL != range);
    assert((0 != item_type_size) || (range_begin >= range_end));
    assert(   (0 == step_size_from_item_to_item)
           || (item_type_size <= step_size_from_item_to_item));
    
    if (NULL == range) {
        return EINVAL;
    }
    
    if (   (0 == item_type_size)
        && (range_begin < range_end)) {
        return EINVAL;
    }
    
    /* If the step size is greater than zero it also must be greater than
     * the item type size.
     * A step size of zero leads to a range whose front is always the same
     * item, which doesn't advance, and therefore seems to contain an
     * infinite number of items of the same value. 
     */
    if (   (0 != step_size_from_item_to_item)
        && (item_type_size > step_size_from_item_to_item)) {
        return EINVAL;
    }
    
    range->step_size_to_next_item = step_size_from_item_to_item;
    range->item_type_size = item_type_size;
    range->begin = (AMP_BYTE *)range_begin;
    range->end = (AMP_BYTE *)range_end;
    
    return AMP_SUCCESS;
}




int amp_byte_range_init_with_item_count(struct amp_byte_range_s *range,
                                        void *range_begin,
                                        size_t item_count,
                                        size_t item_type_size)
{
    return amp_byte_range_init(range, 
                               range_begin, 
                               ((AMP_BYTE *)range_begin) + (item_count * item_type_size),
                               item_type_size, 
                               item_type_size);
}



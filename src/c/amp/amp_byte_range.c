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



void amp_byte_range_copy_from_to(struct amp_byte_range_s const *source,
                                 struct amp_byte_range_s *target)
{
    /* TODO: @todo Rewrite with memcpy. */
    
    assert(NULL != source);
    assert(NULL != target);
    
    target->begin = source->begin;
    target->item_type_size = source->item_type_size;
    target->step_size_to_next_item = source->step_size_to_next_item;
    target->end = source->end;
}



AMP_BOOL amp_byte_range_is_empty(struct amp_byte_range_s const *range)
{
    assert(NULL != range);
    
    return (range->begin + range->item_type_size) > range->end;
}



void* amp_byte_range_get_front(struct amp_byte_range_s const *range)
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



size_t amp_byte_range_advanceable_count(struct amp_byte_range_s const *range)
{
    assert(NULL != range);
    
    if (AMP_TRUE != amp_byte_range_is_empty(range)) {
        
        size_t const step_size_to_next_item = range->step_size_to_next_item;
        
        if (0 != step_size_to_next_item) {
            size_t const range_byte_size = range->end - range->begin;
            
            size_t const account_for_possible_end_gap = range_byte_size - range->item_type_size;
            
            size_t const contained_steps = account_for_possible_end_gap / step_size_to_next_item;
            
            /* + 1 to account for the situation where the range end
             * has enough space for an item but not for a whole item to item 
             * step.
             */
            return contained_steps + 1;
        } else {
            return ~((size_t)0);
        }
    } 
    
    return (size_t)0;
}



/**
 * Returns AMP_TRUE if the range can't be emptied by advancing the front,
 * otherwise AMP_FALSE is returned.
 */
AMP_BOOL amp_byte_range_is_infinitely_advanceable(struct amp_byte_range_s const *range)
{
    assert(NULL != range);
    
    if (   (0 == range->step_size_to_next_item)
        && (AMP_FALSE == amp_byte_range_is_empty(range))) {
        
        return AMP_TRUE;
    }
    
    return AMP_FALSE;
}



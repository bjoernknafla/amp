/*
 *  amp_raw_barrier_generic_broadcast.c
 *  amp
 *
 *  Created by Björn Knafla on 14.05.10.
 *  Copyright 2010 Bjoern Knafla Parallelization + AI + Gamedev Consulting. All rights reserved.
 *
 */

#include "amp_raw_barrier.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include "amp_stddef.h"
#include "amp_mutex.h"



#if !defined(AMP_USE_GENERIC_BROADCAST_BARRIERS)
#   error Compiling wrong source file for selected backend.
#endif



enum amp_internal_raw_barrier_lifecycle_state {
    amp_internal_valid_raw_barrier_lifecycle_state = 0xabcdef
};


int amp_raw_barrier_init(struct amp_raw_barrier_s* barrier,
                         amp_barrier_count_t init_count)
{
    assert(NULL != barrier);
    assert(0 < init_count);
    
    if (NULL == barrier
        || 0 == init_count) {
        return EINVAL;
    }
    
    int errc = amp_raw_mutex_init(&barrier->count_mutex);
    assert(AMP_SUCCESS == errc);
    if (AMP_SUCCESS != errc) {
        return errc;
    }
    
    errc = amp_raw_condition_variable_init(&barrier->waking_condition);
    assert(AMP_SUCCESS == errc);
    if (AMP_SUCCESS != errc) {
        int const ec = amp_raw_mutex_finalize(&barrier->count_mutex);
        assert(AMP_SUCCESS == ec);
        
        return errc;
    }
    
    barrier->count = init_count;
    barrier->init_count = init_count;
    barrier->period = 0;
    barrier->valid = (int)amp_internal_valid_raw_barrier_lifecycle_state;
    
    return AMP_SUCCESS;
}



int amp_raw_barrier_finalize(struct amp_raw_barrier_s* barrier)
{
    assert(NULL != barrier);
    assert((int)amp_internal_valid_raw_barrier_lifecycle_state == barrier->valid);
    if (NULL == barrier
        || (int)amp_internal_valid_raw_barrier_lifecycle_state != barrier->valid) {
        return EINVAL;
    }
    
    int errc = ENOSYS;
    
#if !defined(NDEBUG)
    amp_barrier_count_t barrier_count = 0;
    /* Weak check in debug mode that no threads wait for the barrier. */
    errc = amp_mutex_lock(&barrier->count_mutex);
    assert(AMP_SUCCESS == errc);
    {
        barrier_count = barrier->count;
    }
    errc = amp_mutex_unlock(&barrier->count_mutex);
    assert(AMP_SUCCESS == errc);

    if (barrier_count != barrier->init_count) {
        return EBUSY;
    }
#endif
    
    barrier->valid = ~((int)amp_internal_valid_raw_barrier_lifecycle_state);
    
    errc = amp_raw_condition_variable_finalize(&barrier->waking_condition);
    assert(AMP_SUCCESS == errc);
    
    int errc2 = amp_raw_mutex_finalize(&barrier->count_mutex);
    assert(AMP_SUCCESS == errc2);
    
    /* If one of the finalize functions reported an error there is no
     * way to repair the barrier and eventually resources (mutex or condition
     * variable) will be lost.
     */
    if (AMP_SUCCESS != errc) {        
        return errc;
    } else if (AMP_SUCCESS != errc2) {
        return errc2;
    }
    
    return AMP_SUCCESS;
}


int amp_barrier_wait(amp_barrier_t barrier)
{
    assert(NULL != barrier);
    assert((int)amp_internal_valid_raw_barrier_lifecycle_state == barrier->valid);
    
    if (NULL == barrier
        || (int)amp_internal_valid_raw_barrier_lifecycle_state != barrier->valid) {
        
        return EINVAL;
    }
    
    int return_code = amp_mutex_lock(&barrier->count_mutex);
    assert(AMP_SUCCESS == return_code);
    if (AMP_SUCCESS != return_code) {
        return return_code;
    }
    {
        assert(0 != barrier->count && "Barrier count underflow imminent");
        --(barrier->count);
        amp_barrier_count_t current_count = barrier->count;
        
        
        if (current_count == 0) {
            ++(barrier->period);
            barrier->count = barrier->init_count;
            return_code = amp_raw_condition_variable_broadcast(&barrier->waking_condition);
            if (AMP_SUCCESS == return_code) {
                return_code = AMP_BARRIER_SERIAL_THREAD;
            }
            
        } else {
            amp_barrier_count_t waiting_period = barrier->period;
            
            while (waiting_period == barrier->period) {
                return_code = amp_raw_condition_variable_wait(&barrier->waking_condition,
                                                       &barrier->count_mutex);
                if (AMP_SUCCESS != return_code) {
                    break;
                }
            }
        }
    }
    int errc = amp_mutex_unlock(&barrier->count_mutex);
    assert(AMP_SUCCESS == errc);
    
    
    return return_code;
}

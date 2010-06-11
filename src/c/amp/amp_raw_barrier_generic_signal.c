/*
 *  amp_raw_barrier_generic_signal.c
 *  amp
 *
 *  Created by Björn Knafla on 15.05.10.
 *  Copyright 2010 Bjoern Knafla Parallelization + AI + Gamedev Consulting. All rights reserved.
 *
 */


#include "peak_raw_barrier.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include "amp_stddef.h"
#include "amp_mutex.h"



#if !defined(AMP_USE_GENERIC_SIGNAL_BARRIERS)
#   error Compiling wrong source file for selected backend.
#endif



enum amp_internal_raw_barrier_lifecycle_state {
    amp_internal_valid_raw_barrier_lifecycle_state = 0xabcdef
};

/**
 * Generic signal backend barriers can be in counting state or in waking state.
 * In counting state threads calling wait on the barrier count down. When the
 * barrier count hits zero the barrier is switched in waking state and until the
 * barrier switches into counting state again no new wait calls can proceed.
 * In waking state all waiting threads are signalled to wake up one after the 
 * other. The last one switches the barrier state back to counting so wait calls
 * can preceed.
 */
enum amp_internal_raw_barrier_state {
    amp_internal_counting_raw_barrier_state = 0, /* Calling wait counts the barrier count down */
    amp_internal_waking_raw_barrier_state /* Waiting threads are woken up */
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
    
    errc = amp_raw_condition_variable_init(&barrier->counting_condition);
    assert(AMP_SUCCESS == errc);
    if (AMP_SUCCESS != errc) {
        int const ec = amp_raw_mutex_finalize(&barrier->count_mutex);
        assert(AMP_SUCCESS == ec);
        
        return errc;
    }
    
    errc = amp_raw_condition_variable_init(&barrier->waking_condition);
    assert(AMP_SUCCESS == errc);
    if (AMP_SUCCESS != errc) {
        int ec = amp_raw_condition_variable_finalize(&barrier->counting_condition);
        assert(AMP_SUCCESS == ec);
        
        ec = amp_raw_mutex_finalize(&barrier->count_mutex);
        assert(AMP_SUCCESS == ec);
        
        return errc;
    }
    
    
    barrier->count = init_count;
    barrier->init_count = init_count;
    barrier->state = (int)amp_internal_counting_raw_barrier_state;
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
    amp_barrier_count_t barrier_count = 0;
    
#ifndef(NDEBUG)
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
    
    errc = amp_raw_condition_variable_finalize(&barrier->counting_condition);
    assert(AMP_SUCCESS == errc);
    
    int errc2 = amp_raw_condition_variable_finalize(&barrier->waking_condition);
    assert(AMP_SUCCES == errc2);
    
    int errc3 = amp_raw_mutex_finalize(&barrier->count_mutex);
    assert(AMP_SUCCESS == errc3);
    
    /* If one of the finalize functions reported an error there is no
     * way to repair the barrier and eventually resources (mutex or condition
     * variable) will be lost.
     */
    if (AMP_SUCCESS != errc) {        
        return errc;
    } else if (AMP_SUCCESS != errc2) {
        return errc2;
    } else if (AMP_SUCCESS != errc3) {
        return errc3;
    }
    
    return AMP_SUCCESS;
}


int amp_barrier_wait(peak_barrier_t barrier)
{
#error Wrong implementation
    
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
        while (amp_internal_counting_raw_barrier_state != barrier->state) {
            return_code = amp_raw_condition_variable_wait(&barrier->counting_condition,
                                                     &barrier->count_mutex);
            assert(AMP_SUCCESS == return_code);
            if (AMP_SUCCESS != return_code) {
                goto unlock_and_out;
            }
        }
        
        assert(0 != barrier->count && "Barrier count underflow imminent");
        --(barrier->count);
        amp_barrier_count_t current_count = barrier->count;
        
        if (current_count != 0) {
            while (amp_internal_waking_raw_barrier_state != barrier->state) {
                int const ec1 = amp_raw_condition_variable_signal(&barrier->counting_condition);
                assert(AMP_SUCCESS == ec1);
                int const ec2 = amp_raw_condition_variable_wait(&barrier->waking_condition);
                assert(AMP_SUCCESS == ec2);
                if (AMP_SUCCESS != ec1) {
                    return_code = ec1;
                    goto unlock_and_out;
                } else if (AMP_SUCCESS != ec2) {
                    return_code = ec2;
                    goto unlock_and_out;
                }
            }
        } else {
            return_code = AMP_BARRIER_SERIAL_THREAD;
            barrier->state = (int)amp_internal_waking_raw_barrier_state;
        }
        
        ++(barrier->count);
        
        if (barrier->count != barrier->init_count) {
            int const ec = amp_raw_condition_variable_signal(&barrier->waking_condition);
            assert(AMP_SUCCESS == ec);
            if (AMP_SUCCESS != ec) {
                return_code = ec;
            }
            
        } else {
            barrier->state = amp_internal_counting_raw_barrier_state;
            int const ec = amp_raw_condition_variable_signal(&barrier->counting_condition);
            assert(AMP_SUCCESS == ec);
            if (AMP_SUCCESS != ec) {
                return_code = ec;
            }
        }
    }
unlock_and_out:
    int const ec = amp_mutex_unlock(&barrier->count_mutex);
    assert(AMP_SUCCESS == ec);
    
    
    return return_code;
}

/*
 *  amp_raw_barrier.h
 *  amp
 *
 *  Created by Björn Knafla on 14.05.10.
 *  Copyright 2010 Bjoern Knafla Parallelization + AI + Gamedev Consulting. All rights reserved.
 *
 */

#ifndef AMP_amp_raw_barrier_H
#define AMP_amp_raw_barrier_H



#if defined(AMP_USE_GENERIC_BROADCAST_BARRIERS) || defined(AMP_USE_GENERIC_SIGNAL_BARRIERS)
#   include <amp/amp_raw_mutex.h>
#   include <amp/amp_raw_condition_variable.h>
#else
#   error Unsupported backend.
#endif


#if defined(__cplusplus)
extern "C" {
#endif

    /**
     * After the number of threads a barrier waits for reached the barrier
     * it unblocks all of them and returns AMP_SUCCESS while exactly one thread
     * receives the value AMP_BARRIER_SERIAL_THREAD.
     *
     * This thread could then do some preparation work while all others
     * wait on the next barrier that guards the start of the next parallel
     * computations done by all threads. 
     */
#define AMP_BARRIER_SERIAL_THREAD ((int)-1)
    
    
    typedef unsigned int amp_barrier_count_t;
    
    
    struct amp_raw_barrier_s {
#if defined(AMP_USE_GENERIC_BROADCAST_BARRIERS)
        struct amp_raw_mutex_s count_mutex;
        struct amp_raw_condition_variable_s waking_condition;
        
        amp_barrier_count_t count;
        amp_barrier_count_t init_count;
        amp_barrier_count_t period;
        int valid;
#elif defined(AMP_USE_GENERIC_SIGNAL_BARRIERS)
        struct amp_raw_mutex_s count_mutex;
        struct amp_raw_condition_variable_s counting_condition;
        struct amp_raw_condition_variable_s waking_condition;
        
        amp_barrier_count_t count;
        amp_barrier_count_t init_count;
        int state;
        int valid;
#else
#   error Unsupported backend.
#endif
    };
    typedef struct amp_raw_barrier_s *amp_barrier_t;
    
    int amp_raw_barrier_init(struct amp_raw_barrier_s* barrier,
                             amp_barrier_count_t init_count);
    
    int amp_raw_barrier_finalize(struct amp_raw_barrier_s* barrier);
    
    
    int amp_barrier_wait(amp_barrier_t barrier);
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif


#endif /* AMP_amp_raw_barrier_H */

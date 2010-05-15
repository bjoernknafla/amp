/*
 *  amp_raw_barrier_test.cpp
 *  amp
 *
 *  Created by Björn Knafla on 15.05.10.
 *  Copyright 2010 Bjoern Knafla Parallelization + AI + Gamedev Consulting. All rights reserved.
 *
 */


#include <UnitTest++.h>

#include <cassert>
#include <cerrno>

#include <amp/amp_stddef.h>
#include <amp/amp_raw_barrier.h>



SUITE(amp_raw_barrier)
{
    /* TODO: @todo Remove the asserts for conditions that are also handled
     *             to enable testing of bad usage of barriers.
    TEST(erroneous_init)
    {
        int errc = amp_raw_barrier_init(NULL, 5);
        CHECK_EQUAL(EINVAL, errc);
        
        struct amp_raw_barrier_s barrier;
        errc = amp_raw_barrier_init(&barrier, 0);
        CHECK_EQUAL(EINVAL, errc);
    }
    */

    TEST(init_and_finalize) 
    {
        struct amp_raw_barrier_s barrier;
        int errc = amp_raw_barrier_init(&barrier, 1);
        CHECK_EQUAL(AMP_SUCCESS, errc);
        
        errc = amp_barrier_wait(&barrier);
        CHECK_EQUAL(AMP_BARRIER_SERIAL_THREAD, errc);
        
        errc = amp_raw_barrier_finalize(&barrier);
        CHECK_EQUAL(AMP_SUCCESS, errc);
    }
    
    
} // SUITE(amp_raw_barrier)

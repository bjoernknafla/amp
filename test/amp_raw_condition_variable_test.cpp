/*
 *  amp_raw_condition_variable_test.cpp
 *  amp
 *
 *  Created by Björn Knafla on 28.09.09.
 *  Copyright 2009 Bjoern Knafla Parallelization + AI + Gamedev Consulting. All rights reserved.
 *
 */

/**
 * @file
 *
 * Tests the shallow wrapper around condition variables.
 */

#include <amp/amp_raw_condition_variable.h>



#include <UnitTest++.h>


#include <amp/amp_raw_mutex.h>
#include <amp/amp_raw_semaphore.h>
#include <amp/amp_raw_thread.h>



SUITE(amp_raw_condition_variable)
{
    
    
    TEST(init_and_finalize)
    {
        struct amp_raw_condition_variable_s cond;
        
        int retval = amp_raw_condition_variable_init(&cond);
        CHECK_EQUAL(AMP_SUCCESS, retval);
        
        retval = amp_raw_condition_variable_finalize(&cond);
        CHECK_EQUAL(AMP_SUCCESS, retval);
    }
    
    
    
    
#error Implement
    
    
    
} // SUITE(amp_raw_condition_variable)



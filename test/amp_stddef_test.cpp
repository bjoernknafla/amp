/*
 *  amp_stddef_test.cpp
 *  amp
 *
 *  Created by Björn Knafla on 21.11.09.
 *  Copyright 2009 Bjoern Knafla Parallelization + AI + Gamedev Consulting. All rights reserved.
 *
 */


#include <UnitTest++.h>

#include <stdlib.h>

#include <amp/amp_stddef.h>



SUITE(amp_stddef)
{
    TEST(amp_bool_type)
    {
        AMP_BOOL b = AMP_TRUE;
        CHECK(b);
        
        b = AMP_FALSE;
        CHECK(!b);
        
        amp_bool_t bb = AMP_TRUE;
        CHECK(bb);
        
        bb = AMP_FALSE;
        CHECK(!bb);
        
        CHECK(AMP_TRUE);
        CHECK(!AMP_FALSE);
    }
    
    
    
    TEST(amp_byte_type)
    {
        CHECK(sizeof(AMP_BYTE) * CHAR_BIT == 8);
        CHECK(sizeof(amp_byte_t) * CHAR_BIT == 8);
        
        CHECK(((AMP_BYTE)(~0)) > ((AMP_BYTE)0));
        CHECK(((amp_byte_t)(~0)) > ((amp_byte_t)0));  
    }
    
    
    
} // SUITE(amp_stddef)



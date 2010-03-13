/*
 * Copyright (c) 2009-2010, Bjoern Knafla
 * http://www.bjoernknafla.com/
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are 
 * met:
 *
 *   * Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the Bjoern Knafla 
 *     Parallelization + AI + Gamedev Consulting nor the names of its 
 *     contributors may be used to endorse or promote products derived from 
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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



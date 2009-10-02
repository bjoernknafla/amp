/*
 * Copyright (c) 2009, Bjoern Knafla
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

/**
 * @file
 *
 * Shallow wrapper around Windows Vista condition variables.
 *
 * TODO: @todo Test on Windows Vista system.
 */

#inlude "amp_raw_condition_variable.h"


int amp_raw_condition_variable_init(amp_raw_condition_variable_t cond)
{
    assert(NULL != cond);
    
    if (NULL == cond) {
        return EINVAL;
    }
    
    /* No value returned. */
    InitializeConditionVariable(&cond->cond);
    
    return AMP_SUCCESS;
}



int amp_raw_condition_variable_finalize(amp_raw_condition_variable_t cond)
{
    assert(NULL != cond);
    
    if (NULL == cond) {
        return EINVAL;
    }
    
    /* Nothing to do... */
    
    return AMP_SUCCESS;
}



int amp_raw_condition_variable_broadcast(amp_raw_condition_variable_t cond)
{
    assert(NULL != cond);
    
    /* No value returned. */
    WakeAllConditionVariable(&cond->cond);
    
    return AMP_SUCCESS;
}



int amp_raw_condition_variable_signal(amp_raw_condition_variable_t cond)
{
    assert(NULL != cond);
    
    /* No value returned. */
    WakeConditionVariable(&cond->cond);
    
    return AMP_SUCCESS;
}



int amp_raw_condition_variable_wait(amp_raw_condition_variable_t cond,
                                    amp_raw_mutex_t mutex)
{
    assert(NULL != cond);
    assert(NULL != mutex);
    
    BOOL retval = SleepConditionVariable(&cond->cond, &mutex->critical_section, INFINITE);
    assert(FALSE != retval && "Unexpected error.");
    
    /* GetLastError has more infos in case of error. */
    
    return AMP_SUCCESS;
}



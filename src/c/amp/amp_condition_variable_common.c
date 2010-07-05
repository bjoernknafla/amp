#include "amp_condition_variable.h"

#include <assert.h>
#include <stddef.h>

#include "amp_raw_condition_variable.h"
#include "amp_return_code.h"



int amp_condition_variable_create(amp_condition_variable_t* cond,
                                  amp_allocator_t allocator)
{
    amp_condition_variable_t tmp_cond = AMP_CONDITION_VARIABLE_UNINITIALIZED;
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != cond);
    assert(NULL != allocator);
    
    *cond = AMP_CONDITION_VARIABLE_UNINITIALIZED;
    
    
    tmp_cond = (amp_condition_variable_t)AMP_ALLOC(allocator,
                                                   sizeof(*tmp_cond));
    if (NULL == tmp_cond) {
        return AMP_NOMEM;
    }
    
    retval = amp_raw_condition_variable_init(tmp_cond);
    if (AMP_SUCCESS == retval) {
        *cond = tmp_cond;
    } else {
        int const rc = AMP_DEALLOC(allocator,
                                   tmp_cond);
        assert(AMP_SUCCESS == rc);
        (void)rc;
    }
    
    return retval;
}



int amp_condition_variable_destroy(amp_condition_variable_t* cond,
                                   amp_allocator_t allocator)
{
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != cond);
    assert(NULL != *cond);
    assert(NULL != allocator);
    
    retval = amp_raw_condition_variable_finalize(*cond);
    if (AMP_SUCCESS == retval) {
        retval = AMP_DEALLOC(allocator,
                             *cond);
        assert(AMP_SUCCESS == retval);
        if (AMP_SUCCESS == retval) {
            *cond = AMP_CONDITION_VARIABLE_UNINITIALIZED;
        }
    }
    
    return retval;
}



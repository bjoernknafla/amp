#include "amp_condition_variable.h"

#include <assert.h>
#include <stddef.h>

#include "amp_raw_condition_variable.h"
#include "amp_return_code.h"



int amp_condition_variable_create(amp_condition_variable_t* cond,
                                  void* allocator_context,
                                  amp_alloc_func_t alloc_func,
                                  amp_dealloc_func_t dealloc_func)
{
    amp_condition_variable_t tmp_cond = AMP_CONDITION_VARIABLE_UNINITIALIZED;
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != cond);
    assert(NULL != alloc_func);
    assert(NULL != dealloc_func);
    
    *cond = AMP_CONDITION_VARIABLE_UNINITIALIZED;
    
    
    tmp_cond = (amp_condition_variable_t)alloc_func(allocator_context,
                                                    sizeof(*tmp_cond));
    if (NULL == tmp_cond) {
        return AMP_NOMEM;
    }
    
    retval = amp_raw_condition_variable_init(tmp_cond);
    if (AMP_SUCCESS == retval) {
        *cond = tmp_cond;
    } else {
        int const rc = dealloc_func(allocator_context,
                                    tmp_cond);
        assert(AMP_SUCCESS == rc);
    }
    
    return retval;
}



int amp_condition_variable_destroy(amp_condition_variable_t* cond,
                                   void* allocator_context,
                                   amp_dealloc_func_t dealloc_func)
{
    int retval = AMP_UNSUPPORTED;
    
    assert(NULL != cond);
    assert(NULL != *cond);
    assert(NULL != dealloc_func);
    
    retval = amp_raw_condition_variable_finalize(*cond);
    if (AMP_SUCCESS == retval) {
        retval = dealloc_func(allocator_context,
                              *cond);
        assert(AMP_SUCCESS == retval);
        if (AMP_SUCCESS == retval) {
            *cond = AMP_CONDITION_VARIABLE_UNINITIALIZED;
        }
    }
    
    return retval;
}



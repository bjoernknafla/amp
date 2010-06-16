#include "amp_condition_variable.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include "amp_raw_condition_variable.h"
#include "amp_stddef.h"



int amp_condition_variable_create(amp_condition_variable_t *cond,
                                  void *allocator_context,
                                  amp_alloc_func_t alloc_func,
                                  amp_dealloc_func_t dealloc_func)
{
    assert(NULL != cond);
    assert(NULL != alloc_func);
    assert(NULL != dealloc_func);
    
    if (NULL == cond
        || NULL == alloc_func
        || NULL == dealloc_func) {
        
        return EINVAL;
    }
    
    amp_condition_variable_t tmp_cond = (amp_condition_variable_t)alloc_func(allocator_context,
                                                                             sizeof(struct amp_raw_condition_variable_s));
    if (NULL == tmp_cond) {
        return ENOMEM;
    }
    
    int const retval = amp_raw_condition_variable_init(tmp_cond);
    if (AMP_SUCCESS == retval) {
        *cond = tmp_cond;
    } else {
        int const rc = dealloc_func(allocator_context,
                                    tmp_cond);
        assert(AMP_SUCCESS == rc);
    }
    
    return retval;
}



int amp_condition_variable_destroy(amp_condition_variable_t cond,
                                   void *allocator_context,
                                   amp_dealloc_func_t dealloc_func)
{
    assert(NULL != cond);
    assert(NULL != dealloc_func);
    
    if (NULL == cond
        || NULL == dealloc_func) {
        
        return EINVAL;
    }
    
    int retval = amp_raw_condition_variable_finalize(cond);
    if (AMP_SUCCESS == retval) {
        retval = dealloc_func(allocator_context,
                              cond);
    }
    
    return retval;
}



/*
 *  amp_raw_thread_local_slot_pthreads.c
 *  amp
 *
 *  Created by Björn Knafla on 08.09.09.
 *  Copyright 2009 Bjoern Knafla. All rights reserved.
 *
 */


#include "amp_raw_thread_local_slot.h"


#include <assert.h>
#include <errno.h>


#include "amp_stddef.h"


int amp_raw_thread_local_slot_init(amp_raw_thread_local_slot_key_t *key)
{
    if (NULL == key) {
        assert(NULL != key);
        
        return EINVAL;
    }
    
    int const retval = pthread_key_create(&(key->key), NULL);
    if (0 != retval) {
        assert(EAGAIN != retval && "Insufficient resources or PTHREAD_KEYS_MAX exceeded.");
        assert(ENOMEM != retval && "Insufficient memory to create key.");
        assert(0 == retval && "Unknown error code.");
        
        return retval;
    }
    
    return AMP_SUCCESS;
}



int amp_raw_thread_local_slot_finalize(amp_raw_thread_local_slot_key_t key)
{
    int const retval = pthread_key_delete(key.key);
    if (0 != retval) {
        assert(EINVAL != retval && "Key is invalid.");
        assert(0 == retval && "Unknown error code.");
        
        return retval;
    }
    
    return AMP_SUCCESS;
}



int amp_raw_thread_local_slot_set_value(amp_raw_thread_local_slot_key_t key,
                                                  void *value)
{
    int const retval = pthread_setspecific(key.key, value);
    if (0 != retval) {
        assert(ENOMEM != retval && "");
        assert(EINVAL != retval && "");
        assert(0 == retval && "Unknown error code.");
        
        return retval;
    }
    
    return AMP_SUCCESS;
}



void* amp_raw_thread_local_slot_get_value(amp_raw_thread_local_slot_key_t key)
{
    // TODO: @todo Add a debug status flag to check if a key has been 
    // initialized correctly.
    return pthread_getspecific(key.key);
}



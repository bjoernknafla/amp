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

/**
 * @file
 *
 * Shallow wrapper around Pthreads thread-specific data or Windows thread-local
 * storage.
 *
 * On creation of a thread-local slot a key is returned and each thread creates
 * its own local slot to store data in it. Whatever thread is using the key
 * via the get and set value functions accesses its own local slot.
 *
 * On creation of a new thread it creates local slots for all existing keys.
 *
 * On destruction of a key the thread local slots associated with it from all
 * threads are destroyed, too. The developer has to care for memory and resource
 * management for the values stored in slots as the slot destruction won't care 
 * for the values.
 */

#ifndef AMP_amp_thread_local_slot_H
#define AMP_amp_thread_local_slot_H

#include <stddef.h>

#include <amp/amp_memory.h>



#if defined(__cplusplus)
extern "C" {
#endif

    
#define AMP_THREAD_LOCAL_SLOT_UNINITIALIZED NULL
    
    typedef struct amp_raw_thread_local_slot_key_s *amp_thread_local_slot_key_t;
    
    
    /**
     * Creates a thread-local slot for all threads (including the main thread)
     * whose value can be accessed via the identifier stored in @a key. The 
     * initial value in the slot belonging to key is NULL.
     *
     * Each thread has to set the value of its own slot.
     *
     * If the initialization fails the allocator is called to free the
     * already allocated memory which must not result in an error or otherwise
     * behavior is undefined.
     *
     * @return AMP_SUCCESS on successful slot creation.
     *         AMP_ERROR if system resource are insufficient or the max slot 
     *         count has been exceeded.
     *         AMP_NOMEM if the system has insufficient memory to create the 
     *         key.
     *         Other error codes might be returned to signal errors while
     *         creating, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR if the key can't be stored in the argument.
     *
     * @attention key mustn't be NULL.
     */
    int amp_thread_local_slot_create(amp_thread_local_slot_key_t* key,
                                     amp_allocator_t allocator);
    
    
    /**
     * Invalidates @a key.
     *
     * Destroying a slot doesn't deallocate the memory that might be
     * associated with the data stored in the different thread-local slots,
     * therefore remove the data on all threads first if necessary.
     *
     * @return AMP_SUCCESS on successful finalization.
     *         Error codes might be returned to signal errors while
     *         finalization, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR if the key is invalid.
     *
     * @attention Trying to access a slot via an invalid, e.g. finalized key,
     *            results in undefined behavior.
     */
    int amp_thread_local_slot_destroy(amp_thread_local_slot_key_t* key,
                                      amp_allocator_t allocator);
    
    
    /**
     * Store a value in the slot identified by key.
     *
     * @return AMP_SUCCESS on success.
     *         AMP_NOMEM if the system has insufficient memory to associate the
     *         value with the key.
     *         Error codes might be returned to signal errors while
     *         setting the value, too. These are programming errors and mustn't 
     *         occur in release code. When @em amp is compiled without NDEBUG
     *         set it might assert that these programming errors don't happen.
     *         AMP_ERROR if the key is invalid.
     *
     * @attention NULL is a special value - for @em amp it means that the slot
     *            is empty.
     *
     * @attention Trying to access a slot via an invalid, e.g. finalized key,
     *            results in undefined behavior.
     */
    int amp_thread_local_slot_set_value(amp_thread_local_slot_key_t key,
                                        void* value);
    
    
    /**
     * Query the slot via its key for its value.
     *
     * @return Current value stored in the slot associated with the key for the
     *         thread calling the function or NULL if the slot is empty.
     *
     * @attention Trying to access a slot via an invalid, e.g. finalized key,
     *            results in undefined behavior.
     */
    void* amp_thread_local_slot_value(amp_thread_local_slot_key_t key);
    
    
    
    
#if defined(__cplusplus)
} /* extern "C" */
#endif        


#endif /* AMP_amp_thread_local_slot_H */

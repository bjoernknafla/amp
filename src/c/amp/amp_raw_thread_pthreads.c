#include "amp_raw_thread.h"


// Include assert
#include <assert.h>

// Include EINVAL, ESRCH, EAGAIN, EDEADLK
#include <errno.h>



/// Token for amp_raw_thread_s->state symbolizes thread hasn't launched.
#define AMP_RAW_THREAD_PRELAUNCH_STATE 0x0

/// Token for amp_raw_thread_s->state symbolizes thread has launched.
#define AMP_RAW_THREAD_LAUNCHED_STATE 0xbeeb42

/// Token for amp_raw_thread_s->state symbolizes thread has joined.
#define AMP_RAW_THREAD_JOINED_STATE 0xebbe42


/// A platforms thread function that internally calls the user set 
/// amp_raw_thread_func_t function.
/// Purely internal function.
void* native_thread_adapter_func(void *thread);
void* native_thread_adapter_func(void *thread)
{
    struct amp_raw_thread_s *thread_context = (struct amp_raw_thread_s *)thread;
    
    // Check if this the thread the argument indicates it should be.
    // The thread id can't be tested here as it is only stored after launching
    // / creating the thread.
    // assert(0 != pthread_equal(thread_context->native_thread_description.thread , pthread_self()));
    
    thread_context->thread_func(thread_context->thread_func_context);
        
    return NULL;
}



int amp_raw_thread_launch(amp_raw_thread_t *thread, 
                           void *thread_func_context, 
                           amp_raw_thread_func_t thread_func)
{
    assert(NULL != thread);
    assert(AMP_RAW_THREAD_LAUNCHED_STATE != thread->state);
    assert(AMP_RAW_THREAD_JOINED_STATE != thread->state);
    assert(NULL != thread_func);
    
    if (NULL == thread_func) {
        return EINVAL;
    }
    
    thread->thread_func = thread_func;
    thread->thread_func_context = thread_func_context;
    thread->state = AMP_RAW_THREAD_PRELAUNCH_STATE;
    
    int const retval = pthread_create(&(thread->native_thread_description.thread), 
                                      NULL,
                                      native_thread_adapter_func, 
                                      thread);
    
    // Lewis, Berg, Multithreaded Programming with Pthreads, chapter 11, 
    // Return Values and Error Reporting, pp. 181--184
    assert(EINVAL != retval);
    assert(EAGAIN != retval);
    
    if (0 == retval) {
        thread->state = AMP_RAW_THREAD_LAUNCHED_STATE;
    }
    
    return retval;
}



int amp_raw_thread_join(amp_raw_thread_t *thread)
{
    assert(0 != thread);
    assert(AMP_RAW_THREAD_LAUNCHED_STATE == thread->state);
    
    if (AMP_RAW_THREAD_LAUNCHED_STATE != thread->state) {
        // If thread hasn't been launched it could be already joined or is 
        // invalid.
        if (AMP_RAW_THREAD_JOINED_STATE == thread->state) {
            // Thread has already joined.
            return EINVAL;
        } else {
            // thread doesn't point to valid thread data.
            return ESRCH;
        }
    }
    
    // Currently is ignored.
    void *thread_exit_value = 0;
    int const retval = pthread_join(thread->native_thread_description.thread,
                                    &thread_exit_value);
    
    // Lewis, Berg, Multithreaded Programming with Pthreads, chapter 11, 
    // Return Values and Error Reporting, pp. 181--184
    assert(EINVAL != retval);
    assert(EDEADLK != retval);
    
    if (0 == retval) {
        // Successful join.
        thread->state = AMP_RAW_THREAD_JOINED_STATE;
    }
    
    return retval;
}


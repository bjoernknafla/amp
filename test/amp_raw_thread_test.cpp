
#include <UnitTest++.h>

// Include std::size_t
#include <cstddef>

// Include AMP_SUCCESS
#include <amp/amp_stddef.h>

#include <amp/amp_raw_thread.h>

namespace
{
    int const launch_run_join_success_value = 42;
    
    void launch_run_join_thread_func(void *context)
    {
        int* value_to_set = static_cast<int*>(context);
        *value_to_set = launch_run_join_success_value;
    }
    
    
    
    
    struct parallel_job_processor_func_context_s {
        int id;
        bool did_run;
        int *value_to_set;
    };
    
    
    void parallel_job_processor_func(void *context)
    {
        struct parallel_job_processor_func_context_s *ctxt = 
            static_cast<parallel_job_processor_func_context_s*>(context);
        
        ctxt->did_run = true;
        *(ctxt->value_to_set) = ctxt->id;
    }
    
    
} // anonymous namespace


SUITE(amp_raw_thread)
{
    TEST(launch_run_join)
    {

        
        std::size_t const thread_count = 64;
        
        amp_raw_thread_t threads[thread_count];
        int values_to_set[thread_count] = {0};
        
        for (std::size_t i = 0; i < thread_count; ++i) {
            int retval = amp_raw_thread_launch(&threads[i], 
                                                (void *)&values_to_set[i], 
                                                launch_run_join_thread_func);
            CHECK_EQUAL(AMP_SUCCESS, retval);
        }
        
        for (std::size_t i = 0; i < thread_count; ++i) {
            int retval = amp_raw_thread_join(&threads[i]);
            CHECK_EQUAL(AMP_SUCCESS, retval);
        }
        
        for (std::size_t i = 0; i < thread_count; ++i) {
            CHECK_EQUAL(launch_run_join_success_value, values_to_set[i]);
        }
        
    }
    
    
    TEST(parallel_job_processor)
    {
        std::size_t const concurrencsy_level = 4;
        std::size_t const job_count = 100;
        
        // Set up return values to check if they have the right value 
        // (their index) after job processing.
        int values_to_set[job_count] = {0};
 
        // Set up the individual job contexts
        struct parallel_job_processor_func_context_s job_contexts[job_count];
        for (std::size_t i = 0; i < job_count; ++i) {
            job_contexts[i].id = static_cast<int>(i);
            job_contexts[i].did_run = false;
            job_contexts[i].value_to_set = &values_to_set[i];
        }
        
        
        
        // Create the job processor cluster
        amp_parallel_job_processor_t jp;
        amp_parallel_job_processor_create(&jp, 
                                           concurrency_level, 
                                           NULL, 
                                           amp_malloc);
        
        // Transmit one job per context and value to set to the processing 
        // cluster.
        for (std::size_t i = 0; i < job_count; ++i) {
            amp_transmit_async(jp, 
                                &job_contexts[i], 
                                parallel_job_processor_func);
        }
        
        // Service jobs from the processing cluster as long as jobs are
        // available.
        // @attention If jobs enter new jobs into the cluster this can run
        //            forever.
        amp_parallel_job_processor_drain(jp, AMP_JOB_PROCESSOR_DRAIN_ALL);
        
        // Check that the jobs set the values to set to the job id which
        // should be equal to the value index in its array.
        for (std::size_t i = 0; i < job_count; ++i) {
            CHECK_EQUAL(static_cast<int>(i), job_contexts[i].id);
            CHECK_EQUAL(true, job_contexts[i].did_run);
            CHECK_EQUAL(static_cast<int>(i), values_to_set[i]);
        }
        
        
        // Free the memory and stop all threads used by the processing cluster.
        amp_parallel_job_processor_destroy(jp);
    }
    
    
    
}


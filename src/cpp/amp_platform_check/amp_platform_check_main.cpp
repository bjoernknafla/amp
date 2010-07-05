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
 * Calls amp_platform detection functions and prints their output to stdout.
 */


#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <sstream>


#include <amp/amp.h>



namespace {
    

    void exit_on_error(int error_code)
    {
        if (AMP_SUCCESS != error_code) {
            std::cerr << "amp_platform_check error: " << std::strerror(error_code) << "\n";
            exit(error_code);
        }
    }
    
    
    void exit_on_error_other_than_enosys(int error_code)
    {
        if (AMP_SUCCESS != error_code
            && ENOSYS != error_code) {
            
            std::cerr << "amp_platform_check error: " << std::strerror(error_code) << "\n";
            exit(error_code);
        }
    }
 
    
    std::string count_to_string(std::size_t number)
    {
        std::ostringstream out;
        
        // If a number is @c 0 interpret it as impossible to query.
        
        if (0 != number) {
            out << number;
        } else {
            out << "unsupported";
        }
        
        return out.str();
    }
    
}



int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;    
    
    amp_platform_t platform = AMP_PLATFORM_UNINITIALIZED;
    
    int const error_code_create = amp_platform_create(&platform,AMP_DEFAULT_ALLOCATOR);
    exit_on_error(error_code_create);
    
    // Extract numbers from platform description.
    std::size_t core_count = 0;
    std::size_t active_core_count = 0;
    std::size_t hwthread_count = 0;
    std::size_t active_hwthread_count = 0;

    int error_code = AMP_SUCCESS;
    
    error_code = amp_platform_get_installed_core_count(platform, &core_count);
    exit_on_error_other_than_enosys(error_code);
    
    error_code = amp_platform_get_active_core_count(platform, 
                                                        &active_core_count);
    exit_on_error_other_than_enosys(error_code);
    
    error_code = amp_platform_get_installed_hwthread_count(platform, &hwthread_count);
    exit_on_error_other_than_enosys(error_code);
    
    error_code = amp_platform_get_active_hwthread_count(platform, &active_hwthread_count);
    exit_on_error_other_than_enosys(error_code);
    
    int const error_code_destroy = amp_platform_destroy(&platform,AMP_DEFAULT_ALLOCATOR);
    exit_on_error(error_code_destroy);
    
    
    std::cout << "amp_platform_check\n";
    
    std::cout << "Core count    : " << count_to_string(active_core_count) 
        << "/" << count_to_string(core_count) << " (active/installed)\n";
    
    std::cout << "Hwthread count: " << count_to_string(active_hwthread_count)
        << "/" << count_to_string(hwthread_count) << " (active/installed)\n";
    
    std::cout << "\n\n";
    
    return EXIT_SUCCESS;
}



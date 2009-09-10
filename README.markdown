amp - assemblies for parallelism
--------------------------------

*amp* is a collection of portable low-level *C* assemblies for parallelism and   
threading, for example `amp_thread`, `amp_semaphore`, or `amp_mutex`. Use these  
parallelism atoms to create higher-level parallel solutions by including  
`amp/amp.h`.

All functions and types use the `amp_` prefix, while preprocessor symbols use  
`AMP_`.  

No platform specific headers and symbols are included by `amp/amp.h`. However  
you can access headers containing `_raw_` in their file name. These are  
shallow wrappers around the platforms thread functionality and include platform  
headers.  


The code can be downloaded at: http://github.com/bjoernknafla/amp/
Please file issues with the code at: http://github.com/bjoernknafla/amp/issues


### Author(s) and Contact ###

Bjoern Knafla
Bjoern Knafla Parallelization + AI + Gamedev Consulting
amp@bjoernknafla.com
www.bjoernknafla.com


### Copyright and License ###

*amp* is free software. You may copy, distribute, and modify it under the terms
of the license contained in the file COPYRIGHT.txt distributed with this
package. This license is equal to the Simplified BSD License.

*amp* was developed to parallelize the AiGameDev.com Sandbox and as a 
foundation to experiment and research job pools for computer and video games.
Joint ownership of the copyright belongs to AiGameDev.com.


### Building ###

To build *amp* you have got the following options:

 *  Define AMP_USE_PTHREADS and only compile the generic C source files
    and the C files ending in _pthreads.c to build for Pthreads.
    
 *  Define AMP_USE_PTHREADS and AMP_USE_POSIX_1003_1B_SEMAPHORES and only
    compile generic C source files, C files ending in _pthreads.c, but instead
    of compiling amp_raw_semaphore_pthreads.c use 
    amp_raw_semaphore_posix_1003_b1.c to build for Pthreads but use the
    POSIX 1003 1b semaphores.
    
 *  Define AMP_USE_WINTHREADS and only compile generic C files and C source
    files ending in _winthreads.c to build for Windows threads.


### References ###

 *  David R. Butenhof, Programming POSIX Threads, Addison-Wesley, 1997
 *  Bil Lewis and Danile J. Berg, Multithreaded Programming with Pthreads, 
    Sun Microsystems Press, 1998
 *  Joe Duffy, Concurrent Programming on Windows, Addison-Wesley 2009


### Disclaimer ###

All trademarks copyrights belong to their respective trademark holders and
copyright owners.


### Release Notes ###

#### Version 0.0.1 (September 10, 2009) ####

 *  Initial release






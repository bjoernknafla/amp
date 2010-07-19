amp - assemblies for parallelism
--------------------------------

*amp* is a collection of portable low-level *C* assemblies for parallelism and
threading, for example `amp_thread`, `amp_semaphore`, or `amp_mutex`. Use these
parallelism atoms to create higher-level parallel solutions by including
`amp/amp.h`. *amp* tries to be as minimal as possible to be easily portable
and usable in different projects so they don't have to write this low-level
cross-platform wrappers again and again.


All functions and types use the `amp_` prefix, while preprocessor symbols use
`AMP_`.  


No platform specific headers and symbols are included by `amp/amp.h`. However
you can access headers containing `_raw_` in their file name. These are
shallow wrappers around the platforms thread functionality and include platform
specific headers.


The following platforms are supported: Mac OS X 10.6, Windows (XP and higher),
and platforms supporting POSIX threads (Pthreads). As of yet *amp* is actively
developed and tested on Mac OS X and WindowsXP.


*amp* is developed using a test-driven methodology. Tests rely on the [UnitTest++](http://unittest-cpp.sourceforge.net/)
library by Noel Llopis and Charles Nicholson. Take a look at the source files
in the 'test' subdirectory to get an impression how to use *amp*.


The code can be downloaded at: [http://github.com/bjoernknafla/amp/](http://github.com/bjoernknafla/amp/)  
Please file issues with the code or feature requests at: [http://github.com/bjoernknafla/amp/issues/](http://github.com/bjoernknafla/amp/issues)  


### amp primitives ###

*amp* supports the following cross-platform threading and parallelism 
primitives:

 *  `amp_thread` - launch and join with threads.
 *  `amp_thread_local_slot` - thread specific storage.
 *  `amp_thread_array` - control a whole set of threads.
 *  `amp_mutex` - lock, trylock, or unlock a mutex.
 *  `amp_condition_variable` - signal, broadcast, or wait on a condition 
    variable in combination with a mutex. Works on WindowsXP, too.
 *  `amp_semaphore` - signal or wait on a semaphore.
 *  `amp_barrier` - barrier for a specified number of threads.
 *  `amp_platform` - query the platform for the installed and/or active number
    of processor cores or hardware-threads.


### Usage guidelines ###

Never pass `NULL` or invalid pointers to the *amp* functions if it is not 
specifically documented to be ok.

Never pass non-created/uninitialized values to *amp* functions other than to the 
create and init functions. *amp* has no way to detect invalid pointers
and for performance reasons does not even try to detect them.

Always check return codes that flag errors that can occur, e.g. not enough
memory, errors indicating that a threading primitive count has reached its
max, or return codes from trylocks which indicate that attaining the lock was
not successful.
At least assert on return codes that indicate programming errors, e.g. 
passing out of range count values to create functions.
Many programming errors will result in undefined behavior - read the 
documentation and prevent making these errors, e.g. trying to unlock a mutex
owned by another thread.

Never copy raw primitives! It is ok to have multiple pointers to a single
raw primitive but care for ownership and lifetime management in that case.

*amp* should be used to create higher-level parallelism abstractions, e.g.
job or task pools, but threads shouldn't be used by the typical programmer
working on non-parallelism domain solutions.


### Warning ###

*amp* just started to aggregate in code - and it will change, transform, and
grow from version to version.


### Acknowledgements ###

Many thanks go to the following guys (in order of "appearance", twitter
name in brackets) - you are awesome:

 -  Anthony Williams ([@a_williams](http://twitter.com/a_williams)) and Dimitriy V'jukov ([@dvyukov](http://twitter.com/dvyukov))(names in order of
discussion contacts) for their interest and time to discuss the WindowsXP
condition variable implementation, and their invaluable and awesome feedback
that opened my eyes and made this code more correct and faster. All remaining
errors in the code are mine.

 -  Jedd Orion Haberstro ([@jhaberstro](http://twitter.com/jhaberstro)) for indepth feedback to the documentation of
`amp_thread_yield` which resulted in a clearer explanation of the function.

 -  Rick Molloy ([@rickmolloy](http://twitter.com/rickmolloy)) for insightful discussions and his help to port and test *amp* on Windows 7.


### Author(s) and Contact ###

You have got feedback, questions, criticism, or just want to say hello? I am
looking forward to hear from you!

Bjoern Knafla  
Bjoern Knafla Parallelization + AI + Gamedev Consulting  
email [amp@bjoernknafla.com](mailto:amp@bjoernknafla.com)  
web [www.bjoernknafla.com](http://www.bjoernknafla.com)  
twitter [@bjoernknafla](http://twitter.com/bjoernknafla)  


### Copyright and License ###

*amp* is free software. You may copy, distribute, and modify it under the terms
of the license contained in the file COPYRIGHT.txt distributed with this
package. This license is equal to the Simplified BSD License.


*amp* was developed as a foundation to experiment and research job pools for 
computer and video games and to parallelize the AiGameDev.com Sandbox.


Joint ownership of the copyright belongs to
[Bjoern Knafla](http://www.bjoernknafla.com) and
[AiGameDev.com](http://AiGameDev.com).


### Building ###

See the `BUILDING.txt` file in the root directory of the project for build
instructions.


### References ###

 *  David R. Butenhof, Programming POSIX Threads, Addison-Wesley, 1997
 *  Bil Lewis and Danile J. Berg, Multithreaded Programming with Pthreads, 
    Sun Microsystems Press, 1998
 *  Joe Duffy, Concurrent Programming on Windows, Addison-Wesley 2009
 *  Maurice Herlihy and Nir Shavit, The Art of Multiprocessor Programming, 
    Morgan Kaufmann, 2008
 *  Gadi Taubenfeld, Synchronization Algorithms and Concurrent Programming,
    Pearson Education, 2006


### Disclaimer ###

All trademarks copyrights belong to their respective trademark holders and
copyright owners.


### Release Notes ###

#### Version 0.1.0 (July 06, 2010) ####

 *  Separated raw and non-raw headers. Only raw headers include platform
    specific headers. Use non-raw headers for minimal compile-time dependencies.
 *  Added a platform core count or hardware-threads detection.
 *  Added a simple barrier primitive.
 *  Added thread arrays to simplify creation, destruction, and control of
    a whole set of threads.
 *  Functions do not return errno or POSIX error codes anymore but only return
    codes defined in amp_return_code.h.
 *  Creation and destruction functions use an amp allocator for memory 
    management.
 *  Xcode test and platform check targets build and run different combinations
    of platform backends for easier testing and error detection than running
    all tests by hand.
 *  Made thread id querying internal because its Pthreads implementation is
    currently a hack.
    

#### Version 0.0.1 (September 10, 2009) ####

 *  Initial release






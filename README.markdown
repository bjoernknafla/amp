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


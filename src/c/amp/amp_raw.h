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
 * Main header file includes all raw amp headers. Raw headers are dependent
 * on platform specific headers and therefore include them. Non-raw headers
 * aren't platform dependent and don't include any platform-specific headers.
 *
 * See amp.h and README.markdown for more infos.
 *
 * @attention Don't use amp functions other than the init functions on a
 *            non-initialized data structure.
 * @attention Don't call any amp init function on an already initialized (and
 *            non-finalized) data structure.
 * @attention Never reyl on undefined behavior - as it can vary between versions
 *            of @em amp and the used backend (Pthreads, Windows threads, etc.).
 *
 *
 * TODO: @todo Add include for all non-raw amp headers.
 *
 * TODO: @todo Add Apple OS X 10.6 libdispatch backends for amp_raw_ semaphore
 *             and mutex.
 *
 *
 * TODO: @todo When adding the non-raw data types and connect them to the raw
 *             ones it might make sense to add the work opaque to the raw
 *             data structures.
 *
 * TODO: @todo In winthreads sources store the result of GetLastError in an 
 *             otherwise unused variable to allow inspection of the error code
 *             from the debugger.
 */

#ifndef AMP_amp_raw_H
#define AMP_amp_raw_H


#include <amp/amp_stddef.h>
#include <amp/amp_platform.h>
#include <amp/amp_raw_byte_range.h>
#include <amp/amp_raw_condition_variable.h>
#include <amp/amp_raw_mutex.h>
#include <amp/amp_raw_semaphore.h>
#include <amp/amp_raw_thread.h>
#include <amp/amp_raw_thread_local_slot.h>


#endif /* AMP_amp_raw_H */

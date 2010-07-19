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
 * Main header file includes all non-raw amp headers.
 *
 * See README.markdown for more infos.
 *
 * @em amp is inspired and build after POSXI threads (Pthreads) - though
 * only offers a tiny part of its or Windows thread functionality.
 *
 * Most @em amp functions return an return code while they might also set errno.
 * Always check and rely on the returned error code and don't look at errno.
 * errno might only be set as a sideeffect of calling system APIs.
 * 
 *
 * @attention Don't use amp functions other than the init functions on a
 *            non-initialized data structure.
 * @attention Don't call any amp init function on an already initialized (and
 *            non-finalized) data structure.
 * @attention Never reyl on undefined behavior - as it can vary between versions
 *            of @em amp and the used backend (Pthreads, Windows threads, etc.).
 *
 *
 * TODO: @todo Instead of using assert use an own assertion-macro that allows
 *             abortion only in debug mode or, if a compile time preprocessor
 *             symbol is set, also aborting in non-debug mode, to clearly point
 *             to problem areas in the code instead of hiding them behind
 *             undefined behavior.
 *
 * TODO: @todo Add a section to the readme files how to build @em amp on
 *             the different supported platforms. Mention the need for 
 *             @em UnitTest++ .
 *
 * TODO: @todo Implement setting or using _WIN32_WINNT to include Windows 
 *             version specific functionality correctly.
 *
 * TODO: @todo Document all AMP_USE_ prefixed preprocessor symbols to 
 *             differentiate specific platform backends (Windows Vista
 *             condition variables, Mac OS X 10.6 dispatch semaphores.
 *
 * TODO: @todo Add error preprocessor commands to all platform-ifdefs to 
 *             signal where new platforms need to add code when adapting them.
 */

#ifndef AMP_amp_H
#define AMP_amp_H


#include <amp/amp_stddef.h>
#include <amp/amp_stdint.h>
#include <amp/amp_return_code.h>
#include <amp/amp_memory.h>
#include <amp/amp_platform.h>
#include <amp/amp_thread.h>
#include <amp/amp_thread_array.h>
#include <amp/amp_thread_local_slot.h>
#include <amp/amp_semaphore.h>
#include <amp/amp_mutex.h>
#include <amp/amp_condition_variable.h>
#include <amp/amp_barrier.h>

#endif /* AMP_amp_H */

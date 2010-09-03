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
 * Platform hardware detection via NSProcessInfo.
 *
 * Link with the Cocoa Foundation framework to use this file.
 *
 * amp_platform_create and amp_platform_destroy are implemented in
 * amp_platform_common.c.
 *
 * See http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSProcessInfo_Class/Reference/Reference.html
 *
 */


#include "amp_platform.h"

#include <assert.h>
#include <stddef.h>

#import <foundation/foundation.h>

#include "amp_return_code.h"


/**
 * Helper function to query the active core count.
 */
static size_t amp_internal_platform_get_active_core_count(void);
static size_t amp_internal_platform_get_active_core_count(void)
{
    return (size_t) [[NSProcessInfo processInfo] activeProcessorCount];
}


/**
 * Helper function to query the installed core count.
 */
static size_t amp_internal_platform_get_core_count(void);
static size_t amp_internal_platform_get_core_count(void)
{
    return (size_t) [[NSProcessInfo processInfo] processorCount];
}



int amp_platform_get_installed_core_count(amp_platform_t descr, 
                                    size_t* result)
{
    (void)descr;
    
    assert(NULL != descr);
    
    if (NULL != result ) {
        
        *result = amp_internal_platform_get_core_count();
    }
    
    return AMP_SUCCESS;
}



int amp_platform_get_active_core_count(amp_platform_t descr, 
                                           size_t* result)
{
    (void)descr;
    
    assert(NULL != descr);
    
    if (NULL != result ) {
        
        *result = amp_internal_platform_get_active_core_count();
    }
    
    return AMP_SUCCESS;
}



int amp_platform_get_installed_hwthread_count(amp_platform_t descr, 
                                        size_t* result)
{
    (void)descr;
    
    assert(NULL != descr);
    
    /* Functionality not supported, no value returned. */
    (void)result;
    
    return AMP_UNSUPPORTED;
}



int amp_platform_get_active_hwthread_count(amp_platform_t descr, 
                                               size_t* result)
{
    (void)descr;
    
    assert(NULL != descr);
    
    /* Functionality not supported, no value returned. */
    (void)result;
    
    return AMP_UNSUPPORTED;
}



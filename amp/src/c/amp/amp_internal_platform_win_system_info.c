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

#include "amp_internal_platform_win_system_info.h"

#include <assert.h>
#include <stddef.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "amp_return_code.h"



typedef void (WINAPI *GetNativeSystemInfoFunc)(LPSYSTEM_INFO);



int amp_internal_platform_win_system_info(struct amp_internal_platform_win_info_s* info)
{
    SYSTEM_INFO sysinfo;
    GetNativeSystemInfoFunc get_native_system_info_func;
    DWORD physical_processor_count;
    
    assert(NULL != info);
    
    ZeroMemory(&sysinfo, sizeof(SYSTEM_INFO));
    
    get_native_system_info_func = (GetNativeSystemInfoFunc) GetProcAddress(                                                                                 GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
    
    if (NULL != get_native_system_info_func) {
        get_native_system_info_func(&sysinfo);
    } else {
        GetSystemInfo(&sysinfo);
    }
    
    physical_processor_count = sysinfo.dwNumberOfProcessors;
    
    info->installed_core_count = (size_t)physical_processor_count;
    info->active_core_count = 0;
    info->installed_hwthread_count = 0;
    info->active_hwthread_count = 0;
    
    return AMP_SUCCESS;
}



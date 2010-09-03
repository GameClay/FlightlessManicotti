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
 * Implementation of amp_barrier using amp_mutex and amp_condition_variable
 * and only using condition variable signal to efficiently awake all threads
 * waiting on the barrier when it is fulfilled.
 *
 * amp_barrier_create and amp_barrier_destroy are implemented in 
 * amp_barrier_common.c.
 */

#include "amp_raw_barrier.h"

#include <assert.h>
#include <stddef.h>

#include "amp_stddef.h"
#include "amp_return_code.h"



#if !defined(AMP_USE_GENERIC_SIGNAL_BARRIERS)
#   error Compiling wrong source file for selected backend.
#endif



enum amp_internal_raw_barrier_lifecycle_state {
    amp_internal_valid_raw_barrier_lifecycle_state = 0xabcdef
};

/**
 * Generic signal backend barriers can be in counting state or in waking state.
 * In counting state threads calling wait on the barrier count down. When the
 * barrier count hits zero the barrier is switched in waking state and until the
 * barrier switches into counting state again no new wait calls can proceed.
 * In waking state all waiting threads are signalled to wake up one after the 
 * other. The last one switches the barrier state back to counting so wait calls
 * can preceed.
 */
enum amp_internal_raw_barrier_state {
    amp_internal_counting_raw_barrier_state = 0, /**< Calling wait counts the barrier count down */
    amp_internal_waking_raw_barrier_state /**< Waiting threads are woken up */
};



int amp_raw_barrier_init(amp_barrier_t barrier,
                         amp_barrier_count_t init_count)
{
    int errc = AMP_UNSUPPORTED;
    
    assert(NULL != barrier);
    assert(0 < init_count);
    
    if (0 == init_count) {
        return AMP_ERROR;
    }
    
    errc = amp_raw_mutex_init(&barrier->count_mutex);
    if (AMP_SUCCESS != errc) {
        return errc;
    }
    
    errc = amp_raw_condition_variable_init(&barrier->counting_condition);
    if (AMP_SUCCESS != errc) {
        int const ec = amp_raw_mutex_finalize(&barrier->count_mutex);
        assert(AMP_SUCCESS == ec);
        (void)ec;
        
        return errc;
    }
    
    errc = amp_raw_condition_variable_init(&barrier->waking_condition);
    if (AMP_SUCCESS != errc) {
        int ec = amp_raw_condition_variable_finalize(&barrier->counting_condition);
        assert(AMP_SUCCESS == ec);
        
        ec = amp_raw_mutex_finalize(&barrier->count_mutex);
        assert(AMP_SUCCESS == ec);
        
        return errc;
    }
    
    
    barrier->count = init_count;
    barrier->init_count = init_count;
    barrier->state = (int)amp_internal_counting_raw_barrier_state;
    barrier->valid = (int)amp_internal_valid_raw_barrier_lifecycle_state;
    
    return AMP_SUCCESS;
}



int amp_raw_barrier_finalize(amp_barrier_t barrier)
{
    int errc = AMP_UNSUPPORTED;
    int errc2 = AMP_UNSUPPORTED;
    int errc3 = AMP_UNSUPPORTED;
    amp_barrier_count_t barrier_count = 0;
    
    assert(NULL != barrier);
    assert((int)amp_internal_valid_raw_barrier_lifecycle_state == barrier->valid);
    
    if ((int)amp_internal_valid_raw_barrier_lifecycle_state != barrier->valid) {
        return AMP_ERROR;
    }
    
    
    
#if !defined(NDEBUG)
    /* Weak check in debug mode that no threads wait for the barrier. */
    errc = amp_mutex_lock(&barrier->count_mutex);
    assert(AMP_SUCCESS == errc);
    {
        barrier_count = barrier->count;
    }
    errc = amp_mutex_unlock(&barrier->count_mutex);
    assert(AMP_SUCCESS == errc);
    
    if (barrier_count != barrier->init_count) {
        return AMP_BUSY;
    }
#else
    (void)barrier_count;
#endif
    
    barrier->valid = ~((int)amp_internal_valid_raw_barrier_lifecycle_state);
    
    errc = amp_raw_condition_variable_finalize(&barrier->counting_condition);
    assert(AMP_SUCCESS == errc);
    
    errc2 = amp_raw_condition_variable_finalize(&barrier->waking_condition);
    assert(AMP_SUCCESS == errc2);
    
    errc3 = amp_raw_mutex_finalize(&barrier->count_mutex);
    assert(AMP_SUCCESS == errc3);
    
    /* If one of the finalize functions reported an error there is no
     * way to repair the barrier and eventually resources (mutex or condition
     * variable) will be lost.
     */
    if (AMP_SUCCESS != errc) {        
        return errc;
    } else if (AMP_SUCCESS != errc2) {
        return errc2;
    } else if (AMP_SUCCESS != errc3) {
        return errc3;
    }
    
    return AMP_SUCCESS;
}



int amp_barrier_wait(amp_barrier_t barrier)
{
    int return_code = AMP_UNSUPPORTED;
    int ec = AMP_UNSUPPORTED;
    
    assert(NULL != barrier);
    assert((int)amp_internal_valid_raw_barrier_lifecycle_state == barrier->valid);
    
    if ((int)amp_internal_valid_raw_barrier_lifecycle_state != barrier->valid) {
        
        return AMP_ERROR;
    }
    
    return_code = amp_mutex_lock(&barrier->count_mutex);
    assert(AMP_SUCCESS == return_code);
    if (AMP_SUCCESS != return_code) {
        return return_code;
    }
    {
        amp_barrier_count_t current_count = 0;
        
        while (amp_internal_counting_raw_barrier_state != barrier->state) {
            return_code = amp_condition_variable_wait(&barrier->counting_condition,
                                                      &barrier->count_mutex);
            assert(AMP_SUCCESS == return_code);
            if (AMP_SUCCESS != return_code) {
                goto unlock_and_out;
            }
        }
        
        assert(0 != barrier->count && "Barrier count underflow imminent");
        --(barrier->count);
        current_count = barrier->count;
        
        if (current_count != 0) {
            while (amp_internal_waking_raw_barrier_state != barrier->state) {
                int ec1 = AMP_UNSUPPORTED;
                int ec2 = AMP_UNSUPPORTED;
                
                ec1 = amp_condition_variable_signal(&barrier->counting_condition);
                assert(AMP_SUCCESS == ec1);
                ec2 = amp_condition_variable_wait(&barrier->waking_condition,
                                                  &barrier->count_mutex);
                assert(AMP_SUCCESS == ec2);
                if (AMP_SUCCESS != ec1) {
                    return_code = ec1;
                    goto unlock_and_out;
                } else if (AMP_SUCCESS != ec2) {
                    return_code = ec2;
                    goto unlock_and_out;
                }
            }
        } else {
            return_code = AMP_BARRIER_SERIAL_THREAD;
            barrier->state = (int)amp_internal_waking_raw_barrier_state;
        }
        
        ++(barrier->count);
        
        if (barrier->count != barrier->init_count) {
            int const ec_local = amp_condition_variable_signal(&barrier->waking_condition);
            assert(AMP_SUCCESS == ec_local);
            if (AMP_SUCCESS != ec_local) {
                return_code = ec_local;
            }
            
        } else {
            int ec_local = AMP_UNSUPPORTED;
            barrier->state = amp_internal_counting_raw_barrier_state;
            ec_local = amp_condition_variable_signal(&barrier->counting_condition);
            assert(AMP_SUCCESS == ec_local);
            if (AMP_SUCCESS != ec_local) {
                return_code = ec_local;
            }
        }
    }
unlock_and_out:
    ec = amp_mutex_unlock(&barrier->count_mutex);
    assert(AMP_SUCCESS == ec);
    
    return return_code;
}

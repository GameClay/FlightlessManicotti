//
//  KLAppDelegate.m
//  FlightlessManicotti
//
//  Created by Patrick Wilson on 3/16/12.
//  Copyright (c) 2012 GameClay. All rights reserved.
//

#import "KLAppDelegate.h"

#define KL_ARCH_X64
#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/math/matrix.h>
#include <FlightlessManicotti/math/vector.h>
#include <FlightlessManicotti/scriptinterface/script.h>


// Massive Hax
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <FlightlessManicotti/beat/freq.h>

const int SRATE = 44100;
const int SSIZE = KL_FFT_SZ;

ALshort buffer[44100 * 4];
ALint sample;
ALCdevice *device = NULL;

@implementation KLAppDelegate

@synthesize window = _window;

- (id)init 
{
    self = [super init];
    if (self)
    {
        NSArray *args = [[NSProcessInfo processInfo] arguments]; // TODO
        if(kl_initialize(KL_TRUE, // Threaded script processing
                         KL_FALSE, // Don't wait on script fences
                         "main.lua", // main script
                         0, // No args yet
                         NULL) == KL_SUCCESS)
        {
            [[NSRunLoop currentRunLoop] addTimer:
             [NSTimer timerWithTimeInterval:0.0
                                     target:self
                                   selector:@selector(update:)
                                   userInfo:nil
                                    repeats:YES]
                                    forMode:NSRunLoopCommonModes];
        }
        
        //kl_matrix_math_self_test(); // Hax
        //kl_vector_math_self_test();
        
        // Massive hax
        alGetError();
        device = alcCaptureOpenDevice(NULL, SRATE, AL_FORMAT_STEREO16, SSIZE);
        alcCaptureStart(device);
    }
    return self;
}

- (void)dealloc
{
    [_window release];
    [super dealloc];
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    // Massive hax
    alcCaptureStop(device);
    alcCaptureCloseDevice(device);
    device = NULL;

    kl_predestroy();
    [[NSRunLoop currentRunLoop] addTimer:
     [NSTimer timerWithTimeInterval:0.5
                             target:self
                           selector:@selector(doTerminate:)
                           userInfo:nil
                            repeats:NO]
                            forMode:NSModalPanelRunLoopMode];

    return NSTerminateLater;
}

extern uint64_t hax_sum_frame_time_ns;
extern uint64_t hax_num_frames;
- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    /* slightly moar hax */
    NSLog(@"Average frame time: %f", ((double)hax_sum_frame_time_ns / hax_num_frames) * 1e-6);

    kl_destroy();
}

-(void)doTerminate:(NSTimer *)theTimer
{
    [NSApp replyToApplicationShouldTerminate:YES];
}

-(void)update:(NSTimer *)theTimer
{
    kl_mainloop_iteration();
    
    // Massive hax
    if(device != NULL)
    {
        alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &sample);
        if(sample == SSIZE)
        {
            alcCaptureSamples(device, (ALCvoid *)buffer, sample);
            kl_freq_manager_compute_spectrum(KL_DEFAULT_FREQ_MANAGER, buffer, sample);
        }
    }
}

-(IBAction)onTextEntry:(id)sender
{
    NSString* text = [sender stringValue];
    kl_script_console_input(KL_DEFAULT_SCRIPT_CONTEXT, [text UTF8String]);
    [sender setStringValue:@""];
}

@end

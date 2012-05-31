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

@implementation KLAppDelegate

@synthesize window = _window;

- (id)init 
{
    self = [super init];
    if (self)
    {
        NSArray *args = [[NSProcessInfo processInfo] arguments]; // TODO
        if(kl_initialize(KL_FALSE, // Threaded script processing
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

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    kl_destroy();
}

-(void)doTerminate:(NSTimer *)theTimer
{
    [NSApp replyToApplicationShouldTerminate:YES];
}

-(void)update:(NSTimer *)theTimer
{
    kl_mainloop_iteration();
}

-(IBAction)onTextEntry:(id)sender
{
    NSString* text = [sender stringValue];
    kl_script_console_input(KL_DEFAULT_SCRIPT_CONTEXT, [text UTF8String]);
    [sender setStringValue:@""];
}

@end

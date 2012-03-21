//
//  KLAppDelegate.m
//  FlightlessManicotti
//
//  Created by Patrick Wilson on 3/16/12.
//  Copyright (c) 2012 GameClay. All rights reserved.
//

#import "KLAppDelegate.h"
#include <FlightlessManicotti/fm.h>

@implementation KLAppDelegate

@synthesize window = _window;

- (id)init 
{
    self = [super init];
    if (self)
    {
        NSArray *args = [[NSProcessInfo processInfo] arguments]; // TODO
        if(kl_initialize(KL_FALSE, "main.lua", 0, NULL) == KL_SUCCESS)
        {
            [[NSRunLoop currentRunLoop] addTimer:
             [NSTimer timerWithTimeInterval:0.0
                                     target:self
                                   selector:@selector(update:)
                                   userInfo:nil
                                    repeats:YES]
                                         forMode:NSDefaultRunLoopMode];
        }
    }
    return self;
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

@end
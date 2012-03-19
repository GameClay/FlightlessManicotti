//
//  KLAppDelegate.m
//  FlightlessManicotti
//
//  Created by Patrick Wilson on 3/16/12.
//  Copyright (c) 2012 GameClay. All rights reserved.
//

#import "KLAppDelegate.h"
#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/render/render.h>

@implementation KLAppDelegate

@synthesize window = _window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSArray *args = [[NSProcessInfo processInfo] arguments]; // TODO
    if(kl_initialize(KL_FALSE, "main.lua", 0, NULL) == KL_SUCCESS)
    {
        kl_init_rendering(NULL);
        [[NSRunLoop currentRunLoop] addTimer:
         [NSTimer timerWithTimeInterval:0.0
                                 target:self
                               selector:@selector(update:)
                               userInfo:nil
                                repeats:YES]
                                forMode:NSDefaultRunLoopMode];
    }
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    kl_destroy_rendering();
    kl_destroy();
}

-(void)update:(NSTimer *)theTimer
{
    kl_mainloop_iteration();
}

@end

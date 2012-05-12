//
//  KLAppDelegate.h
//  FlightlessManicotti
//
//  Created by Patrick Wilson on 3/16/12.
//  Copyright (c) 2012 GameClay. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface KLAppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;

-(void)update:(NSTimer *)theTimer;

-(IBAction)onTextEntry:(id)sender;
@end

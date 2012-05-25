//
//  KLOpenGLView.h
//  FlightlessManicotti
//
//  Created by Patrick Wilson on 3/16/12.
//  Copyright (c) 2012 GameClay. All rights reserved.
//

#import <AppKit/AppKit.h>
#import <CoreVideo/CoreVideo.h>

#define KL_ARCH_X64
#include <FlightlessManicotti/render/render.h>

@interface KLOpenGLView : NSOpenGLView
{
    CVDisplayLinkRef displayLink;
    float displayWidth;
    float displayHeight;
    kl_render_context_t renderContext;
}

- (void)dealloc;
- (id)initWithFrame:(NSRect)frame;

@end

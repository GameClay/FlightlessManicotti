//
//  KLOpenGLView.m
//  FlightlessManicotti
//
//  Created by Patrick Wilson on 3/16/12.
//  Copyright (c) 2012 GameClay. All rights reserved.
//

#import "KLOpenGLView.h"
#include <OpenGL/gl.h>

@implementation KLOpenGLView

// This is the renderer output callback function
static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime,
                                      CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
    CVReturn result = [(KLOpenGLView*)displayLinkContext getFrameForTime:outputTime];
    return result;
}

- (id)initWithFrame:(NSRect)frame
{
    NSOpenGLPixelFormatAttribute attrs[] =
    {
        //NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 32,
        0
    };

    NSOpenGLPixelFormat* pixFmt = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];

    self = [super initWithFrame:frame pixelFormat:pixFmt];

    if(!self) return nil;

    return self;
}

- (void)reshape
{
    NSRect bounds = [self bounds];
    displayWidth = NSWidth(bounds);
    displayHeight = NSHeight(bounds);
}

- (void)prepareOpenGL
{
#if 0
    // Synchronize buffer swaps with vertical refresh rate
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
#endif

    // Create a display link capable of being used with all active displays
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);

    // Set the renderer output callback function
    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);

    // Set the display link for the current renderer
    CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = [[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);

    // Init the rendering
    kl_init_rendering(&renderContext, (void*)cglContext);

    NSString *versionString = [NSString stringWithCString:(const char*)glGetString(GL_VERSION)encoding:NSASCIIStringEncoding];
    NSLog(@"OpenGL Version: %@", versionString);

    // Activate the display link
    CVDisplayLinkStart(displayLink);
}

- (CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime
{    
    // draw here
    if(renderContext != NULL)
        kl_render_frame(renderContext, displayWidth, displayHeight);
    
    return kCVReturnSuccess;
}

- (void)dealloc
{
    // Release the display link
    CVDisplayLinkRelease(displayLink);
    
    // Destroy KL rendering
    kl_destroy_rendering(&renderContext);
    
    [super dealloc];
}

@end

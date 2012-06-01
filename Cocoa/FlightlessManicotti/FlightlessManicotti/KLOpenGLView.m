//
//  KLOpenGLView.m
//  FlightlessManicotti
//
//  Created by Patrick Wilson on 3/16/12.
//  Copyright (c) 2012 GameClay. All rights reserved.
//

#import "KLOpenGLView.h"
#include <OpenGL/gl.h>

/* slightly moar hax */
#define KL_OS_DARWIN
#include <FlightlessManicotti/core/timer.h>
uint64_t hax_sum_frame_time_ns = 0;
uint64_t hax_num_frames = 0;

extern BOOL g_loop_last_buffer;

extern BOOL g_shutting_down;

@implementation KLOpenGLView

// This is the renderer output callback function
static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now,
                                      const CVTimeStamp* outputTime, CVOptionFlags flagsIn,
                                      CVOptionFlags* flagsOut, void* displayLinkContext)
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
   kl_render_reshape(renderContext, displayWidth, displayHeight);
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
   // Mainloop iteration
   if(!g_shutting_down)
   {
      kl_mainloop_iteration();
      
      // draw here
      if(renderContext != NULL)
      {
         /* hax */
         kl_absolute_time_t frame_timestamp_start, frame_timestamp_end;
         uint64_t frame_time_ns;
         kl_high_resolution_timer_query(&frame_timestamp_start);
         
         kl_render_frame(renderContext, displayWidth, displayHeight);
         
         /* hax */
         {
            kl_high_resolution_timer_query(&frame_timestamp_end);
            frame_timestamp_end -= frame_timestamp_start;
            kl_absolute_time_to_ns(&frame_timestamp_end, &frame_time_ns);
            hax_sum_frame_time_ns += frame_time_ns;
            hax_num_frames++;
         }
      }
   }
   
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

- (BOOL)acceptsFirstResponder
{
   return YES;
}

-(void)keyDown:(NSEvent*)event
{
   switch([event keyCode])
   {
      case 1: // 's'
         kl_render_spectrum_setting++;
         if(kl_render_spectrum_setting > 5) kl_render_spectrum_setting = 0;
         break;
         
      case 37: // 'l'
         g_loop_last_buffer = !g_loop_last_buffer;
         break;
      default:
         NSLog(@"Key %@", event);
         break;
   }
}

@end

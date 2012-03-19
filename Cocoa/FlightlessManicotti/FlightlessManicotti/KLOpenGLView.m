//
//  KLOpenGLView.m
//  FlightlessManicotti
//
//  Created by Patrick Wilson on 3/16/12.
//  Copyright (c) 2012 GameClay. All rights reserved.
//

#import "KLOpenGLView.h"
#include <OpenGL/gl.h>
#include <FlightlessManicotti/render/render.h>

@implementation KLOpenGLView

-(void) drawRect: (NSRect) bounds
{
    kl_render_frame();
}

@end

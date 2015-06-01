//
//  AppControl.cpp
//  SlideController
//
//  Created by Adolfo López Méndez on 01/06/15.
//
//

#include "AppControl.h"
#include <sstream>

#ifdef __APPLE__
#import <Foundation/Foundation.h>
#import <ApplicationServices/ApplicationServices.h>

    const int AppControl::LEFT_ARROW=123;
    const int AppControl::RIGHT_ARROW=125;
#endif

AppControl::AppControl()
{

    
}

AppControl::~AppControl()
{
    
}

void AppControl::simulate_key(int key_code)
{
#ifdef __APPLE__
    //Simulate keystroke using AppleScript
    std::stringstream command;
    
    std::cout << "Simulating key!" << std::endl;
    command<<"osascript -e 'tell application \"System Events\" to key code "<< key_code << "' ";
    //Run the command as an applescript
    std::system(command.str().c_str());
#endif
}

void AppControl::simulate_mouse( int x, int y, int button,  bool buttonDown)
{
#ifdef __APPLE__
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSUserDefaults *args = [NSUserDefaults standardUserDefaults];
    
    
    
    // The data structure CGPoint represents a point in a two-dimensional
    // coordinate system.  Here, X and Y distance from upper left, in pixels.
    //
    CGPoint pt;
    pt.x = x;
    pt.y = y;
    
    
    // This is where the magic happens.  See CGRemoteOperation.h for details.
    //
    // CGPostMouseEvent( CGPoint        mouseCursorPosition,
    //                   boolean_t      updateMouseCursorPosition,
    //                   CGButtonCount  buttonCount,
    //                   boolean_t      mouseButtonDown, ... )
    //
    // So, we feed coordinates to CGPostMouseEvent, put the mouse there,
    // then click and release.
    //
    CGPostMouseEvent( pt, 1, button, buttonDown );

    
    
    [pool release];

#endif
    
}
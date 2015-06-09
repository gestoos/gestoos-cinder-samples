//
//  AppControl.h
//  SlideController
//
//  Created by Adolfo López Méndez on 01/06/15.
//
//

#ifndef __SlideController__AppControl__
#define __SlideController__AppControl__

#include <iostream>



class AppControl
{
public:
    static const int LEFT_ARROW;
    static const int RIGHT_ARROW;
    
    AppControl();
    
    ~AppControl();
    
    /// Simulate key strokes
    void simulate_key(int key_code);
    
    /// Generate mouse events
    ///
    /// x, y are the mouse coordinates
    /// button is the button id
    /// buttonDown
    ///is true to denote that the mouse is down
    /// To generate a click, you need to send to simulate_mouse events, with button 1, first setting buttonDown to true and then buttonDown to false
    void simulate_mouse(int x, int y, int button=1,  bool buttonDown=false);
    
private:
    
};


#endif /* defined(__SlideController__AppControl__) */

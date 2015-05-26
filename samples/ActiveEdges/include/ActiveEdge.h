//
//  Label.h
//  ActiveEdge
//
//  Created by Mac on 21/05/15.
//
//

#ifndef ActiveEdge_Label_h
#define ActiveEdge_Label_h

#include "cinder/Cinder.h"
#include "cinder/Rand.h"
#include "cinder/app/AppNative.h"
#include "cinder/Text.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

#include "Cinderactor.h"
#include "MovingRect.h"

#include <list>

using namespace ci;
using namespace ci::app;


/// \brief Class implementing an active edge representation
class ActiveEdge {

public:
    
    typedef MovingRect Widget;
    
    /// Empty constructor
    ActiveEdge();
    
    /// Constructor with active edge height
    ActiveEdge(float _h);
    
    /// Default destructor
    virtual ~ActiveEdge();
    
    /// Signal the active edge to show
    void show();
    
    /// Signal the active edge to hide
    void hide();
    
    /// Clear the canvas removing all the widgets
    void clear_canvas();
    
    /// Set the current hand detection
    void set_hand( const gestoos::nui::Hand & h );

    /// NOT USED. Set the cursor position... for debug.
    void set_cursor_pos( const Vec2f & p );
    
    /// Update the current active edge
    void update();
    
    /// Draw the active edge
    void draw() const;
    
    /// Is the active edge being shown or not?
    bool is_showing() const;
    
private:
    
    std::list< Widget > widgets;
    std::list< Widget > canvas_widgets;

    ColorA          color;
    Rectf           rect;
    float           alpha;
    Cinderactor::StrokeType      sense;
    
    gestoos::nui::Hand hand;
    Vec2f hand_pos_f;
    
    bool showing ;

    float show_y, hide_y;
    float show_a, hide_a;
    
    Vec2f show_c, hide_c, center;
    
    Timer timer;
    
    // Reorganize the canvas widgets in a grid. Hardcoded to 3 columns.
    void _reorganize_canvas();
};

#endif

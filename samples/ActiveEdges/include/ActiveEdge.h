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
#include "Widget.h"

#include <list>

using namespace ci;
using namespace ci::app;



class ActiveEdge {

public:
    ActiveEdge();
    
    ActiveEdge(float _h);
    
    virtual ~ActiveEdge();
    
    void show();
    
    void hide();
    
    void reorganize_canvas();

    void set_hand( const gestoos::nui::Hand & h );

    void set_cursor_pos( const Vec2f & p );
    
    void update();
    
    void draw() const;
    
private:
    
    std::list< Widget > widgets;
    std::list< Widget > canvas_widgets;

    ColorA          color;
    Rectf           rect;
    float           alpha;
    Cinderactor::StrokeType      sense;
    
    gestoos::nui::Hand hand;
    
    bool showing ;

    float show_y, hide_y;
    float show_a, hide_a;
    
    Vec2f show_c, hide_c, center;
    
    Timer timer;
};

#endif

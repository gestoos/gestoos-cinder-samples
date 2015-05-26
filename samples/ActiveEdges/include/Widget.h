//
//  Widget.h
//  ActiveEdge
//
//  Created by Mac on 25/05/15.
//
//

#ifndef ActiveEdge_Widget_h
#define ActiveEdge_Widget_h

#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

/// \brief Class implementing a very simple widget representation
class Widget : public Rectf{
public:
    
    /// Empty constructor
    Widget();
    
    /// Constructor with position, size and color
    Widget( const Vec2f & _o, const Vec2f & _s, const ColorA & _c );
    
    /// Default destructor
    virtual ~Widget();
    
    /// Check if point hovers widget. Updates hover internal var.
    bool is_hover( const Vec2f & p );
    
    /// Check if point.x hovers widget's width. Updates hover internal var.
    bool is_x_inside( const Vec2f & p );
    
    /// Signal the widget to show
    void show();
   
    /// Signal the widget to hide
    void hide();
   
    /// Set the position of the widget (will animate to the new position)
    void set_pos( const Vec2f & p );
    
    /// Add an offset to the widget's position (will animate to the new position)
    void set_offset( const Vec2f & o );
  
    /// Update the widget
    void update();
    
    /// Update the widget with a new position
    void update( const Vec2f & c );
    
    /// Draw the widget
    void draw() const;
    
    /// The widget color
    ColorA          w_color;

private:
    Vec2f           w_offset;
    bool hover;
    float alpha_dest, alpha;
    Vec2f pos_dest, pos;
    
    void _init(const Vec2f & _o, const Vec2f & _s, const ColorA & _c);
};

#endif

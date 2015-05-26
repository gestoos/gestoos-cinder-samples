//
//  MovingRect.h
//  ActiveEdge
//
//  Created by Mac on 26/05/15.
//
//

#ifndef ActiveEdge_MovingRect_h
#define ActiveEdge_MovingRect_h

#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

/// \brief Class implementing a very simple Moving Rectangle representation
class MovingRect : public Rectf{
public:
    
    /// Empty constructor
    MovingRect();
    
    /// Constructor with position, size
    MovingRect( const Vec2f & _o, const Vec2f & _s, const ColorA & _c );
    
    /// Default destructor
    virtual ~MovingRect();
    
    /// Check if point hovers MovingRect. Updates hover internal var.
    bool is_hover( const Vec2f & p );
    
    /// Check if point.x hovers MovingRect's width. Updates hover internal var.
    bool is_x_inside( const Vec2f & p );
    
    /// Signal the MovingRect to show
    void show();
    
    /// Signal the MovingRect to hide
    void hide();
    
    /// Set the position of the MovingRect (will animate to the new position)
    void set_pos( const Vec2f & p );
    
    /// Get the current position. Can be animating.
    const Vec2f & get_pos();
    
    /// Add an offset to the MovingRect's position (will animate to the new position)
    void set_offset( const Vec2f & o );
    
    /// Set the current color
    void set_color( const ColorA & c );
    
    /// Get the current color.
    const ColorA & get_color();
    
    /// Update the MovingRect
    virtual void update();
    
    /// Draw the MovingRect.
    virtual void draw() const;
    
    void set_highlight( bool h );

    bool get_highlight();


protected:
    bool    hover;
    bool    highlight;
    
    float   alpha_dest, alpha;
    Vec2f   pos_dest,   pos;
    ColorA  r_color;
    
    
private:
    void _init(const Vec2f & _o, const Vec2f & _s, const ColorA & _c);
};


#endif

//
//  MovingRect.cpp
//  ActiveEdge
//
//  Created by Mac on 26/05/15.
//
//

#include "MovingRect.h"

MovingRect::MovingRect()
{
    _init( Vec2f(0,0), Vec2f(100,100), ColorA(0,0,0,0) );
}

MovingRect::MovingRect( const Vec2f & _o, const Vec2f & _s, const ColorA & _c )
{
    _init( _o, _s, _c );
}


MovingRect::~MovingRect()
{}


bool MovingRect::is_hover( const Vec2f & p )
{
    hover = this->contains( p );
    return hover;
}

bool MovingRect::is_x_inside( const Vec2f & p )
{
    hover = ( p.x >= this->getX1() && p.x <= this->getX2() );
    return hover;
}

void MovingRect::show()
{
    alpha_dest = 1.0;
}

void MovingRect::hide()
{
    alpha_dest = 0.0;
}

void MovingRect::set_pos( const Vec2f & p )
{
    pos_dest = p;
}

const Vec2f & MovingRect::get_pos()
{
    return pos;
}


void MovingRect::set_offset( const Vec2f & o )
{
    pos_dest += o;
}


void MovingRect::set_highlight( bool h )
{
    highlight = h;
}


void MovingRect::set_hover( bool h )
{
    hover = h;
}


bool MovingRect::get_highlight(  )
{
    return highlight;
}


void MovingRect::set_color( const ColorA & c )
{
    r_color = c;
}

const ColorA & MovingRect::get_color()
{
    return r_color;
}

void MovingRect::update()
{
    alpha += (alpha_dest - alpha) * 0.175;
    
    if( hover || highlight )
        r_color.a = 0.9 * alpha;
    else
        r_color.a = 0.5 * alpha;
    
    pos += (pos_dest - pos) * 0.175;
    this->offsetCenterTo( pos /*+ w_offset*/ );

}


void MovingRect::draw() const
{
    gl::color ( r_color );
    gl::drawSolidRoundedRect	( *this, 10.0, 32 );
}


void MovingRect::_init(const Vec2f & _o, const Vec2f & _s, const ColorA & _c)
{
    this->set(0, 0, _s.x, _s.y); //just to set size
    // w_offset = _o;
    pos_dest = _o;
    pos = _o;
    this->offsetCenterTo( pos_dest ); //hard setting center position here
    r_color = _c;
    hover = false;
    highlight = false;
    
    alpha = 0.0;
    alpha_dest = 0.0;
}

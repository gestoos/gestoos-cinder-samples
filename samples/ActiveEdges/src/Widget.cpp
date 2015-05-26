//
//  Widget.cpp
//  ActiveEdge
//
//  Created by Mac on 25/05/15.
//
//

#include "Widget.h"

Widget::Widget()
{
    _init( Vec2f(0,0), Vec2f(100,100), ColorA(0,0,0,0) );
}

Widget::Widget( const Vec2f & _o, const Vec2f & _s, const ColorA & _c )
{
    _init( _o, _s, _c );
}


Widget::~Widget()
{}


bool Widget::is_hover( const Vec2f & p )
{
    hover = this->contains( p );
    return hover;
}

bool Widget::is_x_inside( const Vec2f & p )
{
    hover = ( p.x >= this->getX1() && p.x <= this->getX2() );
    return hover;
}

void Widget::show()
{
    alpha_dest = 1.0;
}

void Widget::hide()
{
    alpha_dest = 0.0;
}

void Widget::set_pos( const Vec2f & p )
{
    pos_dest = p;
}

void Widget::set_offset( const Vec2f & o )
{
    pos_dest += o;
}

void Widget::update()
{
    
    this->offsetCenterTo( pos /*+ w_offset*/ );
    
    alpha += (alpha_dest - alpha) * 0.175;
    
    if( hover )
    {
        w_color.a = 0.9 * alpha;
    }
    else
    {
        w_color.a = 0.5 * alpha;
    }
    pos += (pos_dest - pos) * 0.175;
}

void Widget::update( const Vec2f & c )
{
    // std::cout<<" setting offset to "<<_c + getCenter()<<std::endl;
    pos_dest = c;
    
    update();
    
}

void Widget::draw() const
{
    gl::color ( w_color );
    gl::drawSolidRoundedRect	( *this, 10.0, 32 );
}


void Widget::_init(const Vec2f & _o, const Vec2f & _s, const ColorA & _c)
{
    this->set(0, 0, _s.x, _s.y); //just to set size
    // w_offset = _o;
    pos_dest = _o;
    pos = _o;
    this->offsetCenterTo( pos_dest ); //hard setting center position here
    w_color = _c;
    hover = false;
    alpha = 0.0;
    alpha_dest = 0.0;
}

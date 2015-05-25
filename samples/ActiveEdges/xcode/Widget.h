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


class Widget : public Rectf{
public:
    Widget()
    {
        _init( Vec2f(0,0), Vec2f(100,100), ColorA(0,0,0,0) );
    }
    
    Widget( const Vec2f & _o, const Vec2f & _s, const ColorA & _c )
    {
        _init( _o, _s, _c );
    }
    
    
    ~Widget()
    {}
    
//    void set( const Vec2f & _o, const Vec2f & _s, const ColorA & _c )
//    {
//        _init( _o, _s, _c );
//    }

    bool is_hover( const Vec2f & p )
    {
        hover = this->contains( p );
        return hover;
    }

    bool is_x_inside( const Vec2f & p )
    {
        hover = ( p.x >= this->getX1() && p.x <= this->getX2() );
        return hover;
    }
    
    void show()
    {
        alpha_dest = 1.0;
    }
    
    void hide()
    {
        alpha_dest = 0.0;
    }
    
    void set_pos( const Vec2f & p )
    {
        pos_dest = p;
    }
    
    void set_offset( const Vec2f & o )
    {
        pos_dest += o;
    }

    void update()
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

    void update( const Vec2f & c )
    {
       // std::cout<<" setting offset to "<<_c + getCenter()<<std::endl;
        pos_dest = c;
        
        update();

    }
    
    void draw() const
    {
        gl::color ( w_color );
        gl::drawSolidRoundedRect	( *this, 10.0, 32 );
    }
    
    ColorA          w_color;

private:
    Vec2f           w_offset;
    bool hover;
    float alpha_dest, alpha;
    Vec2f pos_dest, pos;
    
    void _init(const Vec2f & _o, const Vec2f & _s, const ColorA & _c)
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
};

#endif

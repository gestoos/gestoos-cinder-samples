//
//  Slider.cpp
//  Slider
//
//  Created by Mac on 26/05/15.
//
//

#include "Slider.h"

/// Empty constructor
Slider::Slider(){}

/// Constructor with Slider position and size
Slider::Slider(const Vec2f & _p, const Vec2f & _s, const ColorA & _c ) : Rectf( _p-_s/2, _p+_s/2 )
{
    color = _c;
    showing = true;
    alpha = alpha_dest = 1.0;
    pctg = 0.5;
}

/// Default destructor
Slider::~Slider(){}


void Slider::show()
{
    showing = true;
    alpha_dest = 1.0;
}

void Slider::hide()
{
    showing = false;
    alpha_dest = 0.0;
}

bool Slider::is_showing() const{
    return showing;
}


void Slider::set_pctg(float _p)
{
    pctg += ( constrain<float>(_p, 0.05, 1.0) - pctg ) * 0.25;
}

float Slider::get_pctg() const
{
    return pctg;
}

void Slider::update()
{
    alpha += (alpha_dest - alpha) * 0.1;
    color.a = alpha * 0.6;
}

/// Draw the Slider
void Slider::draw( int sense ) const
{
    if(showing)
    {
        gl::color( ColorA( 0.2,0.3,0.4, alpha) );
        gl::drawSolidRoundedRect( *this, 5.0, 32 );
        
        gl::color( color );
 
        Rectf pctg_rect;
        
        if( sense == 0 )
        {
            pctg_rect = Rectf( Vec2f( getX1(), getY1() ), Vec2f( getX1() + pctg*getWidth(), getY2() ) ) ;
        }
        else
        {
            pctg_rect = Rectf( Vec2f( getX1(), getY2() - pctg*getHeight() ), Vec2f( getX2(), getY2() ) ) ;
        }
        gl::drawSolidRoundedRect( pctg_rect, 5.0, 32);
    }
}

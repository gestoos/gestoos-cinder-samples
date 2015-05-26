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
}

/// Default destructor
Slider::~Slider(){}


void Slider::show()
{
    showing = true;
}

void Slider::hide()
{
    showing = false;
}

bool Slider::is_showing() const{
    return showing;
}


void Slider::set_pctg(float _p)
{
    pctg += (fmin(fmax( 0.05, _p ), 1.0) - pctg ) * 0.25;
}

float Slider::get_pctg() const
{
    return pctg;
}

/// Draw the Slider
void Slider::draw( int sense ) const
{
    if(showing)
    {
        std::cout<<"showing"<<std::endl;
        gl::color( ColorA( 0.2,0.3,0.4, 1.0) );
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

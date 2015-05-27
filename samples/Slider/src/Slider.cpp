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
Slider::Slider(const Vec2f & _p, const Vec2f & _s, const ColorA & _c, int _t, Type _tp ) : Rectf( _p-_s/2, _p+_s/2 )
{
    color   = _c;
    showing = true;
    pctg    = 0.5;
    trigger = _t;
    type    = _tp;
    alpha   = alpha_dest    = 1.0;
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

void Slider::set_trigger(int _t)
{
    trigger = _t;
}

int Slider::get_trigger() const
{
    return trigger;
}

void Slider::set_type(Type _t)
{
    type = _t;
}

Slider::Type Slider::get_type() const
{
    return type;
}


void Slider::update()
{
    alpha += (alpha_dest - alpha) * 0.1;
    color.a = alpha * 0.6;
}

/// Draw the Slider
void Slider::draw() const
{
    if(showing)
    {
        if( type == HORIZONTAL )
        {
            gl::color( ColorA( 0.2,0.3,0.4, alpha) );
            gl::drawSolidRoundedRect( *this, 5.0, 32 );
          
            Rectf pctg_rect = Rectf( Vec2f( getX1(), getY1() ), Vec2f( getX1() + pctg*getWidth(), getY2() ) ) ;
            gl::color( color );
            gl::drawSolidRoundedRect( pctg_rect, 5.0, 32);
        }
        else if( type == VERTICAL )
        {
            gl::color( ColorA( 0.2,0.3,0.4, alpha) );
            gl::drawSolidRoundedRect( *this, 5.0, 32 );
           
            Rectf pctg_rect = Rectf( Vec2f( getX1(), getY2() - pctg*getHeight() ), Vec2f( getX2(), getY2() ) ) ;
            gl::color( color );
            gl::drawSolidRoundedRect( pctg_rect, 5.0, 32);
        }
        else if( type == CIRCULAR )
        {
            gl::color( ColorA( 0.2,0.3,0.4, alpha) );
            _drawArc( getCenter(), getWidth(), getHeight(), 0.0, 2*3.14159     , 10 );
            gl::color( color );
            _drawArc( getCenter(), getWidth(), getHeight(), 0.0, 2*3.14159*pctg, 7 );
        }
    }
}

// implementation by http://forum.libcinder.org/topic/arc-in-cinder
void Slider::_drawArc(Vec2f center, int width, int height, float start, float stop, int lw, int res) const
{
    float hw = width / 2.0f;
    float hh = height / 2.0f;
    float inc = 1.0f/(float)res;
    
    int numVertex = 2 * (stop - start)/inc + 3;
    int vertCount = 0;
    
    GLfloat *verts = new float[numVertex];
    for (float i = start; i <= stop; i += inc) {
        
        verts[vertCount++] = center.x + cos(i) * hw;
        
        verts[vertCount++] = center.y - sin(i) * hh;
        
    }
    verts[vertCount++] = center.x + cos(stop) * hw;
    verts[vertCount] = center.y - sin(stop) * hh;
    
    glEnable( GL_LINE_SMOOTH );
    glLineWidth( lw );
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 2, GL_FLOAT, 0, verts );
    glDrawArrays( GL_LINE_STRIP, 0, numVertex/2);
    glDisableClientState( GL_VERTEX_ARRAY );
    delete [] verts;
}
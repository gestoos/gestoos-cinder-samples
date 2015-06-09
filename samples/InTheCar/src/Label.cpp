//
//  Label.cpp
//  InTheCar
//
//  Created by Mac on 21/05/15.
//
//

#include "Label.h"

static int label_count = 0;

Label::Label(){}

Label::Label(const std::string & _t, float _w , CinderDrive::StrokeType _sense)
{
    
    //inherited methods
    text( _t );
    size( _w, TextBox::GROW );
    font( Font( "Arial", 60 ) );
    alignment(  TextBox::CENTER  );
    setColor( ColorA( 0.8f, 0.9f, 1.0f, alpha ) );
    setBackgroundColor( ColorA( 0.3, 0.3, 0.3, alpha ) );
    
    //my vars
    position = Vec2f(ci::app::getWindowWidth()/2.0, getWindowHeight()/2.0) - measure()/2.0 ; //centered
    position.y += Rand::randInt(-30,30); //artistic random Y
    sense = _sense;
    dead = false;
    alpha = 1.0;
    
    idnum = label_count++;
    
    X_TARGET_RIGHT = getWindowWidth();
    X_TARGET_LEFT = 0;
    Y_TARGET_UP = 0;
    TARGET_ALPHA = 0.0;
    
    mTextTexture = gl::Texture( this->render() );
    
    std::cout<< idnum <<" +++ Label born "<<_t<<std::endl;
    
}

Label::~Label()
{
    std::cout<< idnum <<" --- Label RIP "<<std::endl;
}


void Label::set_sense( CinderDrive::StrokeType _s )
{
    sense = _s;
}


void Label::signal_kill()
{
    dead = true;
}

bool Label::is_dead() const
{
    return dead;
}

void Label::update()
{
    //update pos
    if( sense == Cinderactor::RIGHT )
    {
        position.x += ( X_TARGET_RIGHT - position.x ) * 0.08;
    }
    else if( sense == Cinderactor::LEFT )
    {
        position.x += ( X_TARGET_LEFT - position.x ) * 0.08;
    }
    else if( sense == Cinderactor::UP )
    {
        position.y += ( Y_TARGET_UP - position.y ) * 0.08;
    }
    
    //Update alpha
    alpha += (TARGET_ALPHA - alpha) * 0.08;
    setBackgroundColor( ColorA( 1.0, 1.0, 1.0, alpha ) );
    setColor( ColorA( 0.8f, 0.9f, 1.0f, alpha ) );
    mTextTexture = gl::Texture( this->render() );
    
    
    if( std::fabs( position.x - X_TARGET_RIGHT ) < 2 ||
       std::fabs( position.x - X_TARGET_LEFT ) < 2 ||
       std::fabs( position.y - Y_TARGET_UP ) < 2)
    {
        dead = true;
    }
}

void Label::draw() const
{
    if( mTextTexture )
		gl::draw( mTextTexture, position );
}
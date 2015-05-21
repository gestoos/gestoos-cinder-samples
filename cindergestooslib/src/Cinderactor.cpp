//
//  Cinderactor.cpp
//  InTheCar
//
//  Created by Mac on 21/05/15.
//
//

#include "Cinderactor.h"


Cinderactor::Cinderactor() 
{
    block_timer.start();
}

Cinderactor::~Cinderactor(){}


Cinderactor::StrokeType Cinderactor::detect_hand_stroke( int gest )
{
    if( get_hands().first.is_present() )
    {
        int hand_gesture = get_hands().first.get_gesture();
        Vec2f hand_vel = Vec2f( get_hands().first.get_vel().x , get_hands().first.get_vel().y );
        
        if( (hand_gesture == gest || hand_gesture == -1 ) && block_timer.getSeconds() > BLOCK_TIMEOUT )
        {
            if( hand_vel.y < -6.0 && std::fabs( hand_vel.x ) < 4.0  )
            {
                block_timer.start();
                return UP;
            }
            else if( hand_vel.y > 6.0 && std::fabs( hand_vel.x ) < 4.0  )
            {
                block_timer.start();
                return DOWN;
            }
            else if( hand_vel.x > 6.0 && std::fabs( hand_vel.y ) < 4.0)
            {
                block_timer.start();
                return RIGHT;
            }
            else if( hand_vel.x < -6.0 && std::fabs( hand_vel.y ) < 4.0 )
            {
                block_timer.start();
                return LEFT;
            }
        }
    }
    
    return NONE;
}


void Cinderactor::draw() const
{
    float scale = 3.0;
    
    if( get_hands().first.is_present() )
    {
        if( get_hands().first.get_gesture() == GEST_VICTORY )
            gl::color( Color(1.0, 0.6, 0.7));
        else
            gl::color( Color(0.5, 0.6, 0.7));
        
        gl::lineWidth(1.0);
        gl::drawStrokedCircle( Vec2f(70,70), scale*6.0, 32);
        gl::lineWidth(3.0);
        gl::drawLine( Vec2f(70,70), Vec2f(70,70) + scale*Vec2f( get_hands().first.get_vel().x , get_hands().first.get_vel().y ) );
        
        std::stringstream ss;
        ss<<"gesture "<<get_hands().first.get_gesture();
        gl::drawStringCentered(	ss.str(), Vec2f( 70, 30), Color(1.0, 1.0, 1.0) );
    }
    else
    {
        gl::color( Color(0.3, 0.4, 0.5));
        gl::lineWidth(1.0);
        gl::drawStrokedCircle( Vec2f(70,70), scale*6.0, 32);
        
        gl::drawStringCentered(	"no hand", Vec2f( 70, 30), Color(0.3, 0.4, 0.5) );
    }
    
    //Set color back to white
    gl::color( Color(1.0, 1.0, 1.0));
    
}

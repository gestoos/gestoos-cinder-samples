//
//  CinderDrive.cpp
//  InTheCar
//
//  Created by Adolfo López Méndez on 28/05/15.
//
//

#include <stdio.h>

#include "CinderDrive.h"


CinderDrive::CinderDrive()
{
    block_timer.start();
    init_ok = false;
}

CinderDrive::~CinderDrive(){}

void CinderDrive::init(const std::string & ini_file )
{
    gestoos::nui::DriverInteraction::init( ini_file );
    init_ok = true;
}

CinderDrive::StrokeType CinderDrive::detect_hand_stroke( int gest, float timeout )
{
    if( get_hand().is_present() )
    {
        int hand_gesture = get_hand().get_gesture();
        Vec2f hand_vel = Vec2f( get_hand().get_vel().x , get_hand().get_vel().y );
        
        if( (hand_gesture == gest || hand_gesture == -1 ) && block_timer.getSeconds() >= timeout )
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

void CinderDrive::draw() const
{
    // Loading message
    if( !init_ok )
    {
        gl::drawStringCentered(	"Loading models... please wait...", Vec2f( cinder::app::getWindowWidth()/2,cinder::app::getWindowHeight()*0.5 ) );
        
        return;
    }
    
    draw_hand_representation( get_hand(),   Vec2f(200, 70 ) );
    //draw_hand_representation( get_hands().second,  Vec2f(70,  70 ) );
    
}

void CinderDrive::draw_hand_representation( const gestoos::nui::Hand & hand, const Vec2f & where )
{
    float scale = 3.0;
    
    if( hand.is_present() )
    {
        if( hand.get_gesture() == GEST_VICTORY )
        gl::color( Color(1.0, 0.6, 0.7));
        else if( hand.get_gesture() == GEST_EL )
        gl::color( Color(0.5, 1.0, 0.7));
        else
        gl::color( Color(0.5, 0.6, 0.7));
        
        gl::lineWidth(1.0);
        gl::drawStrokedCircle( where, scale*6.0, 32);
        gl::lineWidth(3.0);
        gl::drawLine( where, where + scale*Vec2f( hand.get_vel().x , hand.get_vel().y ) );
        
        std::stringstream ss;
        ss<<"gesture "<<hand.get_gesture();
        gl::drawStringCentered(	ss.str(), where - Vec2f( 0, 40), Color(1.0, 1.0, 1.0) );
    }
    else
    {
        gl::color( Color(0.3, 0.4, 0.5));
        gl::lineWidth(1.0);
        gl::drawStrokedCircle( where, scale*6.0, 32);
        
        gl::drawStringCentered(	"no hand", where - Vec2f( 0, 40), Color(0.3, 0.4, 0.5) );
    }
    
    //Set color back to white
    gl::color( Color(1.0, 1.0, 1.0));
    
}

void CinderDrive::draw_hand_circle( const gestoos::nui::Hand & hand ) const
{
    if( hand.is_present() )
    {
        gl::color( Color(0.3, 0.4, 0.5));
        gl::drawStrokedCircle( Vec2f( hand.get_pos().x * cinder::app::getWindowWidth() / 320.0,  hand.get_pos().y * cinder::app::getWindowHeight() / 240.0), 15.0, 32  );
    }
}
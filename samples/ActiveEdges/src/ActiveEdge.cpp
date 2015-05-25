//
//  ActiveEdge.cpp
//  ActiveEdge
//
//  Created by Mac on 21/05/15.
//
//

#include "ActiveEdge.h"

static int label_count = 0;


ActiveEdge::ActiveEdge()
{
    
}

ActiveEdge::ActiveEdge(float _h)
{
    
    //my vars
    show_y = getWindowHeight() - _h  ;
    hide_y = getWindowHeight() * 1.01  ;
    
    show_c = Vec2f( getWindowWidth()/2., show_y + _h/2  );
    hide_c = Vec2f( getWindowWidth()/2., hide_y + _h/2  );
    
    center = hide_c; //hidden
    
    show_a = 0.7;
    hide_a = 0.3;
    alpha = hide_a;
    
    showing = false;
    
    color = ColorA(0.3,0.4,0.5,0.7);
    
    widgets.push_back( Widget( center + Vec2f(-300,0), Vec2f(100,100), ColorA( 1.0, 0, 0, 1.0) ) );
    widgets.push_back( Widget( center + Vec2f(-100,0), Vec2f(100,100), ColorA( 1.0, 1.0, 0, 1.0) ) );
    widgets.push_back( Widget( center + Vec2f(100,0),  Vec2f(100,100), ColorA( 1.0, 0, 1.0, 1.0) ) );
    widgets.push_back( Widget( center + Vec2f(300,0),  Vec2f(100,100), ColorA( 0.0, 1.0, 1.0, 1.0) ) );
    
    // Set initial rect pos
    rect = Rectf( Vec2f(0,hide_y), Vec2f(getWindowWidth(),hide_y+_h) );
    
    timer.start();
    
}

ActiveEdge::~ActiveEdge()
{
}


void ActiveEdge::show()
{
    if( showing ) return;
    
    showing = true;
    for (auto it=widgets.begin(); it!=widgets.end(); ++it)
    {
        it->set_offset( Vec2f(0,-rect.getHeight()) );
        it->show();
    }
    std::cout<<"SHOW"<<std::endl;
}

void ActiveEdge::hide()
{
    if( !showing ) return;
    
    showing = false;
    for (auto it=widgets.begin(); it!=widgets.end(); ++it)
    {
        it->set_offset( Vec2f(0,rect.getHeight()) );
        it->hide();
    }
    std::cout<<"HIDE"<<std::endl;
}

void ActiveEdge::set_hand( const gestoos::nui::Hand & h )
{
    hand = h;

    if( hand.is_present() && showing )
    {
        
        Vec2f hpos ;
        hpos.x = ( hand.get_pos().x / 320.0 - 0.5 ) * cinder::app::getWindowWidth() *2.0 +   cinder::app::getWindowWidth()/2.0 ;//  / 320.0;
        hpos.y = hand.get_pos().y     *   cinder::app::getWindowHeight()    / 240.0;
        
        for (auto it=widgets.begin(); it!=widgets.end(); ++it)
        {
            // if L gesture, create a new widget
            if( it->is_x_inside( hpos ) &&
                hand.get_gesture() == GEST_EL &&
                timer.getSeconds() > 1.5 )
            {
                std::cout<<" new! "<<std::endl;
                Vec2f new_pos = Vec2f( Rand::randInt(100,getWindowWidth()-100), Rand::randInt(100,getWindowHeight()-200) );
                float new_side = Rand::randInt(80, 120);
                
                Widget new_widget( new_pos, Vec2f(new_side, new_side), it->w_color );
                new_widget.show();
                canvas_widgets.push_back( new_widget );
                
                if( canvas_widgets.size() > 6 )
                    canvas_widgets.pop_front();
                
                timer.start();
            }
        }
    }
    
    if( hand.is_present() && !showing )
    {
        if( hand.get_gesture() == GEST_RELEASE &&
            timer.getSeconds() > 1.0 )
        {
            reorganize_canvas();
        }

    }

    
    // If no hands, hide
    if( !hand.is_present() && showing )
    {
        hide();
    }
   
}

void ActiveEdge::reorganize_canvas()
{
    int cx = 1;
    int cy = 1;
    int padx = getWindowWidth()/4;
    int pady = getWindowHeight()/4;
    for (auto it=canvas_widgets.begin(); it!=canvas_widgets.end(); ++it)
    {
        it->set_pos( Vec2f( cx * padx, cy * pady + 40) );
        cx++ ;
        if( cx > 3 )
        {
            cx = 1;
            cy++;
        }
    }
}

void ActiveEdge::update()
{
    //update pos
    if( showing )
    {
        center += (show_c - center) * 0.2;
        color.a += (show_a - color.a) * 0.1;
    }
    else
    {
        center += (hide_c - center) * 0.2;
        color.a += (hide_a - color.a) * 0.1;
    }
    
    rect.offsetCenterTo( center );
    
    for (auto it=widgets.begin(); it!=widgets.end(); ++it)
    {
        it->update( );
    }
    
    for (auto it=canvas_widgets.begin(); it!=canvas_widgets.end(); ++it)
    {
        it->update();
    }
    
    
    //    cinder::TextBox::setBackgroundColor( ColorA( 1.0, 1.0, 1.0, alpha ) );
}

void ActiveEdge::draw() const
{
    gl::color ( this->color );
    gl::drawSolidRoundedRect( rect, 4.0, 32 );
    
    for (auto it=widgets.begin(); it!=widgets.end(); ++it)
    {
        it->draw();
    }
    for (auto it=canvas_widgets.begin(); it!=canvas_widgets.end(); ++it)
    {
        it->draw();
    }
    
    if( showing && hand.is_present() )
    {
        Vec2f hpos ;
        hpos.x = ( hand.get_pos().x / 320.0 - 0.5 ) * cinder::app::getWindowWidth() *2.0 +   cinder::app::getWindowWidth()/2.0 ;//hand.get_pos().x     *   cinder::app::getWindowWidth()     / 320.0;
        hpos.y = center.y ;
        gl::color( ColorA( 0.6,0.7,0.8,0.6));
        gl::drawSolidCircle( hpos, 20.0, 32 );
    }
}
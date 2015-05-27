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
    snapmode = true;
    
    currW = 0;
    
    color = ColorA(0.2,0.3,0.4,0.7);
    
    int pad = getWindowWidth()/5;
    
    widgets.push_back( Widget( center + Vec2f(-1.5*pad,   0), Vec2f(100,100), ColorA( 1.0, 0.3, 0.3, 1.0) ) );
    widgets.push_back( Widget( center + Vec2f(-0.5*pad,   0), Vec2f(100,100), ColorA( 1.0, 1.0, 0.3, 1.0) ) );
    widgets.push_back( Widget( center + Vec2f( 0.5*pad,   0),  Vec2f(100,100), ColorA( 1.0, 0.3, 1.0, 1.0) ) );
    widgets.push_back( Widget( center + Vec2f( 1.5*pad,   0),  Vec2f(100,100), ColorA( 0.3, 1.0, 1.0, 1.0) ) );
    
    // Set initial rect pos
    rect = Rectf( Vec2f(0,hide_y), Vec2f(getWindowWidth(),hide_y+_h) );
    
    timer.start();
    
}

ActiveEdge::~ActiveEdge()
{
}


void ActiveEdge::show()
{
    if( showing || timer.getSeconds() < 1.0 ) return;
    
    showing = true;
    for (auto it=widgets.begin(); it!=widgets.end(); ++it)
    {
        it->set_offset( Vec2f(0,-rect.getHeight()) );
        it->show();
    }
    timer.start();
    std::cout<<"SHOW"<<std::endl;
}

void ActiveEdge::hide()
{
    if( !showing || timer.getSeconds() < 1.0 ) return;
    
    showing = false;
    for (auto it=widgets.begin(); it!=widgets.end(); ++it)
    {
        it->set_offset( Vec2f(0,rect.getHeight()) );
        it->hide();
    }
    timer.start();
    std::cout<<"HIDE"<<std::endl;
}





void ActiveEdge::snap_on_closest(Vec2f & hand_pos_inst , const gestoos::nui::Hand & h )
{
    // Assume widgets are ordered from left to right of the screen
    
    float dmin = 1000000;
    
    Widget *wmin;
    Widget *wcurr;
    
    std::vector<float> dists(widgets.size());

    std::size_t minid=0;

    std::size_t count=0;
    for (auto it=widgets.begin(); it!=widgets.end(); ++it)
    {
        it->set_hover(false);
        if(count != currW) it->set_highlight(false);
        else wcurr = &(*it);
        float d = std::abs(((it->getX1() + it->getWidth()/2.0) ) - hand_pos_inst.x);
        if(d < dmin)
        {
            dmin = d;
            wmin = &(*it);
            minid = count;
        }
        
        dists[count] = d;
        count++;
        
    }
    
    if(wmin != nullptr)
    {
        
        if(minid == currW )
        {
            wmin->set_highlight(true);
            wcurr = wmin;
        }
        else
        {

//            std::cout << " minid " << minid << " currW " << currW << " dmin " << dmin << " dists[currW] " << dists[currW] << std::endl;

            if(dmin < (0.8*dists[currW]))
            {

                wmin->set_highlight(true);
                currW = minid;
                wcurr = wmin;

                //std::cout << "change " << minid << std::endl;

                
            }
        }
        
        
        
        
        
        // if L gesture, create a new widget
        if(
           h.get_gesture() == GEST_EL &&
           timer.getSeconds() > 1.5 )
        {
            std::cout<<" * New widget! "<<std::endl;
            Vec2f new_pos = Vec2f( Rand::randInt(100,getWindowWidth()-100), Rand::randInt(100,getWindowHeight()-200) );
            float new_side = Rand::randInt(80, 120);
            
            Widget new_widget( new_pos, Vec2f(new_side, new_side), wcurr->get_color() );
            new_widget.show();
            canvas_widgets.push_back( new_widget );
            
            if( canvas_widgets.size() > 6 )
                canvas_widgets.pop_front();
            
            timer.start();
        }

        
    }
    
}



void ActiveEdge::set_hand( const gestoos::nui::Hand & h )
{
    hand = h;

    if( hand.is_present() && showing )
    {
        // Instant hand position scaled to screen
        Vec2f hand_pos_inst ;
        hand_pos_inst.x = ( hand.get_pos().x / 320.0 - 0.5 ) * cinder::app::getWindowWidth() *2.0 +   cinder::app::getWindowWidth()/2.0 ;
        hand_pos_inst.y = hand.get_pos().y     *   cinder::app::getWindowHeight()    / 240.0;
        
        // Filter hand
        hand_pos_f += ( hand_pos_inst - hand_pos_f ) * 0.3 ;
        
        if(snapmode)
        {
            snap_on_closest(hand_pos_f, h);
 
        }
        else
        {
            
            for (auto it=widgets.begin(); it!=widgets.end(); ++it)
            {
                // if L gesture, create a new widget
                if( it->is_x_inside( hand_pos_f ) &&
                   hand.get_gesture() == GEST_EL &&
                   timer.getSeconds() > 1.5 )
                {
                    std::cout<<" * New widget! "<<std::endl;
                    Vec2f new_pos = Vec2f( Rand::randInt(100,getWindowWidth()-100), Rand::randInt(100,getWindowHeight()-200) );
                    float new_side = Rand::randInt(80, 120);
                    
                    Widget new_widget( new_pos, Vec2f(new_side, new_side), it->get_color() );
                    new_widget.show();
                    canvas_widgets.push_back( new_widget );
                    
                    if( canvas_widgets.size() > 6 )
                        canvas_widgets.pop_front();
                    
                    timer.start();
                }
            }

            
            
        }
        
    }
    
    if( hand.is_present() && !showing )
    {
        if( hand.get_gesture() == GEST_RELEASE &&
            timer.getSeconds() > 1.0 )
        {
            _reorganize_canvas();
        }

    }

    
    // If no hands, hide active edge
    if( !hand.is_present() && showing )
    {
        hide();
    }
   
}

void ActiveEdge::change_mode()
{
    snapmode = !snapmode;
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
        it->update();
    }
    
    for (auto it=canvas_widgets.begin(); it!=canvas_widgets.end(); ++it)
    {
        it->update();
    }
    
    
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
    
    if( showing && hand.is_present() && (!snapmode))
    {
        Vec2f hpos ;
        hpos.x = hand_pos_f.x;
        hpos.y = center.y ; // horizontal
        
        gl::color( ColorA( 0.6,0.7,0.8,0.6));
        gl::drawSolidCircle( hpos, 20.0, 32 );
    }
}

bool ActiveEdge::is_showing() const
{
    return showing;
}

void ActiveEdge::_reorganize_canvas()
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
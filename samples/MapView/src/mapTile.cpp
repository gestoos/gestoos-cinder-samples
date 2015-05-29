//
//  mapTile.cpp
//  mapView
//
//  Created by Marcel Alcoverro on 27/05/15.
//
//

#include "mapTile.h"




//
//  MapTile.cpp
//  MapTile
//
//  Created by Mac on 21/05/15.
//
//

#include "MapTile.h"
#include "Resources.h"


static int label_count = 0;


const int MODE_PAN = 0;
const int MODE_ZOOM = 1;
const int MODE_IDLE = 2;
const int MODE_CURSOR =3;



MapTile::MapTile()
{
    
}

MapTile::MapTile(float _h)
{
    
    
    image = gl::Texture(loadImage(loadResource(MAP_IMG)));
    
    
    maporigin.x = cinder::app::getWindowWidth()/2;
    maporigin.y = cinder::app::getWindowHeight()/2;
    
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
    
    track_mode = MODE_IDLE;
    
    currW = 0;
    
    zoom = 0.5;
    offset_zoom = 0.0;
    
    color = ColorA(0.2,0.3,0.4,0.7);
    
    int pad = getWindowWidth()/5;
    

    
    
    sliders.push_back( Slider( getWindowSize()/2 , Vec2f(30, 500),  ColorA(0.9,0.4,0.5,0.5), GEST_EL,  Slider::VERTICAL )  );

    
    hide_all_sliders();
    
    // Set initial rect pos
    rect = Rectf( Vec2f(0,hide_y), Vec2f(getWindowWidth(),hide_y+_h) );
    
    timer.start();
    
    
    //init pointers
    ref_pos = cv::Point3f(-1,-1,-1);
    empty_hand = new gestoos::nui::Hand();
    empty_hand2 = new gestoos::nui::Hand();
    empty_hand->clear();
    empty_hand2->clear();
    hand_g = empty_hand;
    hand_slider = empty_hand2;

    std::cout<<" hand_g present "<<hand_g->is_present()<<std::endl;
    
}

MapTile::~MapTile()
{
    delete empty_hand;
    delete empty_hand2;
}



void MapTile::set_hands( const std::pair<gestoos::nui::Hand, gestoos::nui::Hand> & h )
{
    hands = h;
//    std::cout<<"sh hand_g present "<<hand_g->is_present()<<std::endl;

    if( hands.first.is_present() && !hands.second.is_present() && showing )
    {
        gestoos::nui::Hand & hand = hands.first;
        // Instant hand position scaled to screen
        Vec2f hand_pos_inst ;
//        hand_pos_inst.x = ( hand.get_pos().x / 320.0 - 0.5 ) * cinder::app::getWindowWidth() *2.0 +   cinder::app::getWindowWidth()/2.0 ;
//        hand_pos_inst.y = hand.get_pos().y     *   cinder::app::getWindowHeight()    / 240.0;

                hand_pos_inst.x = ( hand.get_unit_pos().x - 0.5 ) * cinder::app::getWindowWidth()*2.0 + cinder::app::getWindowWidth()/2.0;
                hand_pos_inst.y = (hand.get_unit_pos().y -0.5)    *   cinder::app::getWindowHeight()*2.0 + cinder::app::getWindowHeight()/2.0;

        track_mode = MODE_CURSOR;
        
        // Filter hand
        hand_pos_f += ( hand_pos_inst - hand_pos_f ) * 0.3 ;
        
        if(hand.get_gesture() == GEST_EL)
        {
            track_mode = MODE_PAN;

            
             float mapW = cinder::app::getWindowWidth();
            float mapH = cinder::app::getWindowHeight();
            
            float panBorderx = 0.4;
            float panBordery = 0.45;
            
            float panStep   = 0.03;
            
            
            
            
//            if( (hand_pos_f.x > mapW*(1.0-panBorderx))&& (hand_pos_f.x  < mapW*panBorderx) )
//            {
//                
//            }
            
            
            if(hand_pos_f.x > mapW*(1.0-panBorderx)) 	maporigin.x -= 5.0;//( mapW * panStep - panx ) * 0.05 ;
            if(hand_pos_f.y  > (mapH*(1.0-panBordery))) 	maporigin.y -= 5.0;//( mapH * panStep - pany ) * 0.05 ;
            if(hand_pos_f.x  < mapW*panBorderx) 			maporigin.x += 5.0;//( (-mapW * panStep) - panx ) * 0.05 ;
            if(hand_pos_f.y  < (mapH*panBordery)) 			maporigin.y += 5.0;// ( (-mapH * panStep) - pany ) * 0.05 ;
            
//            if( (currx < mapW*(1.0-panBorderx))&& (currx > mapW*panBorderx) )
//            {
//                panx = 0;
//            }
//            
//            if( (curry < (mapH*(1.0-panBordery) - 300)) && (curry > (mapH*panBordery - 400)) )
//            {
//                pany = 0;
//            }

            
            
            
        }
    }
//    std::cout<<"sh2 hand_g present "<<hand_g->is_present()<<std::endl;

//    else if (hands.first.is_present() && hands.second.is_present()
//                 && hands.first.get_gesture()==GEST_EL && hands.second.get_gesture()==GEST_EL)
//        {
//            
//            if (track_mode==MODE_ZOOM)
//            {
//                float v1 = hands.first.get_vel().x;
//                float v2 = hands.second.get_vel().x;
//                float move1 = hands.first.get_pos().x+hands.first.get_vel().x;
//                float move2 =hands.second.get_pos().x+hands.second.get_vel().x;
//                
//                //std::cout << v1 << " " << v2 << std::endl;
//                if ( fabs(v1-v2) > 6.0 && fabs(move1-move2) < fabs(hands.first.get_pos().x - hands.second.get_pos().x ) )
//                {
//                    //ZOOM OUT
////                    float elapsed_zoom = (float)( clock()-zoom_timeout )/CLOCKS_PER_SEC;
////                    if (elapsed_zoom > 0.5 )
////                    {
////                        myop->callzoomOut();
////                        zoom_timeout = clock();
////                    }
//                    
//                    
//                    if(zoomtimer.getSeconds() > 0.5)
//                    {
//                        zoomtimer.start();
//                        zoom -= 30.0;
//                    }
//                    
//                    
//                }
//                else if ( fabs(v1-v2) > 6.0 && fabs(move1-move2) > fabs(hands.first.get_pos().x - hands.second.get_pos().x ) )
//                {
//                    //ZOOM IN
////                    float elapsed_zoom = (float)( clock()-zoom_timeout )/CLOCKS_PER_SEC;
////                    if (elapsed_zoom > 0.5 )
////                    {
////                        myop->callzoomIn();
////                        zoom_timeout = clock();
////                    }
//                    if(zoomtimer.getSeconds() > 0.5)
//                    {
//                        zoomtimer.start();
//                        zoom += 30.0;
//                    }
//
//                    
//                    
//                }
//                else
//                {
//                    //track_mode=MODE_IDLE;
//                    
//                    //zoom = 0;
//                }
//            }
//            else
//            {
//                
//                track_mode=MODE_ZOOM;
//                //Set zoom pos
//                
//                //As first hand
////                xpos = hands.first.get_unit_pos().x-0.5;
////                ypos = hands.first.get_unit_pos().y-0.5;
////                
////                zoomAt = QPoint(xpos*resolutionX + resolutionX/2, ypos*resolutionY + resolutionY/2);
//                
////                zoom_timeout = clock();
//                    zoomtimer.start();
//
//                //zoom = 0;
//            
//            }
//        }
//

    
}

void MapTile::change_mode()
{
    snapmode = !snapmode;
}


void MapTile::update()
{
    std::cout<<"1 hand_g present "<<hand_g->is_present()<<std::endl;
   
    //    slider_hor.update();
    //    slider_ver.update();
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
        it->update();
    
    gestoos::nui::Hand hand1 = hands.first;
    gestoos::nui::Hand hand2 = hands.second;
    
    // start slider
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
    {
        if(  hand1.is_present() && hand1.get_gesture() == it->get_trigger()  && hand2.is_present() && std::abs(hand1.get_pos().y - hand2.get_pos().y) < 4 )
        {
            
            
            
            track_mode = MODE_ZOOM;
            hand_g =        &hand1;
            hand_slider =   &hand2;
            ref_pos =       hand_g->get_pos();
            hide_all_sliders();

            std::cout<<"showing hand slider "<<it->get_trigger()<<std::endl;
            zoomtimer.start();
            offset_zoom = zoom - 0.5;
            it->set_pctg( zoom );
//            std::cout<<"2 hand_g present "<<hand_g->is_present()<<std::endl;

            break;
        }
        if(  hand2.is_present() && hand2.get_gesture() == it->get_trigger() && hand1.is_present() && std::abs(hand1.get_pos().y - hand2.get_pos().y) < 4  )
        {
            track_mode = MODE_ZOOM;

            hand_g =        &hand2;
            hand_slider =   &hand1;
            ref_pos =       hand_g->get_pos();
            hide_all_sliders();

            std::cout<<"showing hand slider "<<it->get_trigger()<<std::endl;
            zoomtimer.start();
            offset_zoom = zoom - 0.5;
            it->set_pctg( zoom );
//            std::cout<<"3 hand_g present "<<hand_g->is_present()<<std::endl;

            break;
            
        }
    }
    
    std::cout<<"3.1 hand_g present "<<hand_g->is_present()<<std::endl;

    // exit slider
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
    {
        
        if( it->is_showing() &&
           it->get_trigger() > 10 && ( !hand1.is_present() || !hand2.is_present() ) )
        {
            track_mode = MODE_IDLE;

            hide_all_sliders();
            hand_g =empty_hand;
            hand_slider = empty_hand2;
            std::cout<<"quitting "<<it->get_trigger()<<std::endl;
            break;
        }
    }
    
   std::cout<<"4 hand_g present "<<hand_g->is_present()<<std::endl;
    // filter ref pos
    if( hand_g->is_present() )
        ref_pos += (hand_g->get_pos() - ref_pos) * 0.1;
    
    // update pctg
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
    {
        if( it->is_showing() && hand_slider->is_present() )
        {
            float new_pctg = 0.0;
            
            if( it->get_type() == Slider::HORIZONTAL )
            {
                if( hand_slider->get_pos().x > hand_g->get_pos().x )
                    new_pctg = lmap<float>( hand_slider->get_pos().x - ref_pos.x,    60,  120, 0.0, 1.0 );
                else
                    new_pctg = lmap<float>( hand_slider->get_pos().x - ref_pos.x,  -120,  -60, 0.0, 1.0 );
            }
            else if( it->get_type() == Slider::VERTICAL )
            {
                new_pctg = lmap<float>( ( hand_slider->get_pos().y - ref_pos.y ), -40, 40, 1.0, 0.0 );
            }
            else if( it->get_type() == Slider::CIRCULAR )
            {
                std::cout<<hand_slider->get_pos().y<<" - "<<ref_pos.y<<std::endl;
                new_pctg = lmap<float>( ( hand_slider->get_pos().y - ref_pos.y ), -10, 60, 1.0, 0.0 );
            }
            
           
            it->set_pctg( (new_pctg + offset_zoom)  ) ;
            break;
        }
    }
    
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
    {
        if(track_mode == MODE_ZOOM  && it->is_showing())
        {
            //track_mode = MODE_ZOOM;
            float step = 1.0 + std::abs( offset_zoom )*2.0 ;
            zoom =  it->get_pctg() * step;
        }
    }

    
}


void MapTile::hide_all_sliders()
{
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
        it->hide();
}

void MapTile::draw() const
{
    //gl::color ( this->color );
    //gl::drawSolidRoundedRect( rect, 4.0, 32 );
    
    
        
    

    
   // Rectf mpos(0.0,0.0,image.getWidth() + zoom,image.getHeight() + zoom);
     Rectf mpos(0.0,0.0,image.getWidth(),image.getHeight());
    mpos.offsetCenterTo(Vec2f(maporigin.x,maporigin.y));//,image.getWidth(),image.getHeight());
    
    
    mpos.scale(Vec2f(1.0 + (zoom - 0.5) , 1.0 + (zoom - 0.5)));
    
    	gl::draw(image,mpos );
    
    
    if( showing && hands.first.is_present())
    {
        Vec2f hpos ;
        hpos.x = hand_pos_f.x;
        hpos.y = hand_pos_f.y; // horizontal
        
        if(track_mode == MODE_PAN || track_mode == MODE_ZOOM)
        {
            gl::color( ColorA( 0.6,0.7,0.8,0.6));

            gl::drawSolidCircle( hpos, 20.0, 32 );
        }
        else if(track_mode == MODE_CURSOR)
        {
            gl::color( ColorA( 0.6,0.0,0.0,0.6));

            gl::drawSolidCircle( hpos, 20.0, 32 );
        }
        
        
//        if(hands.first.is_present() && hands.second.is_present())
//        {
//            if(track_mode == MODE_ZOOM)
//            {
//                gl::color( ColorA( 0.6,0.0,0.0,0.6));
//                Rectf rpos(0.0,0.0, 200.0 , 200.0);
//                rpos.offsetCenterTo(Vec2f(cinder::app::getWindowWidth()/2,cinder::app::getWindowHeight()/2));//,image.getWidth(),image.getHeight());
//
//                gl::drawStrokedRoundedRect	( rpos, 10.0, 32 );
//
//            }
//        }
        
    }
    
    
    
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
        it->draw();

    
}

bool MapTile::is_showing() const
{
    return showing;
}


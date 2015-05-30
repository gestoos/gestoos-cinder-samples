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

void MapTile::init()
{
    
    
    image = gl::Texture(loadImage(loadResource(MAP_IMG)));
    
    std::cout << "Image size " << image.getSize() << std::endl;
    
    maporigin.x = cinder::app::getWindowWidth()/2;
    maporigin.y = cinder::app::getWindowHeight()/2;
    
    //my vars
    
    track_mode = MODE_IDLE;
    
    currW = 0;
    
    zoom = 0.5;
    offset_zoom = 0.0;
    
    color = ColorA(0.2,0.3,0.4,0.7);
    
    int pad = getWindowWidth()/5;
    

    
    
    sliders.push_back( Slider( Vec2f(getWindowWidth()*0.1, getWindowHeight()/2) , Vec2f(30, 500),  ColorA(0.9,0.4,0.5,0.5), GEST_EL,  Slider::VERTICAL )  );

    
    hide_all_sliders();
    
    
    timer.start();
    
    
    //init pointers
    ref_pos = cv::Point3f(-1,-1,-1);
    empty_hand = new gestoos::nui::Hand();
    empty_hand2 = new gestoos::nui::Hand();
    empty_hand->clear();
    empty_hand2->clear();
    hand_g = empty_hand;
    hand_slider = empty_hand2;

    //std::cout<<" hand_g present "<<hand_g->is_present()<<std::endl;
    
}

MapTile::~MapTile()
{
    delete empty_hand;
    delete empty_hand2;
}


void MapTile::update(const std::pair<gestoos::nui::Hand, gestoos::nui::Hand> & h)
{
 //   std::cout<<"1 hand_g present "<<hand_g->is_present()<<std::endl;
 
    
    
    
    hands = h;

    
    
    // manage Cursor and Pan
    
    if( hands.first.is_present() && !hands.second.is_present())
    {
        gestoos::nui::Hand & hand = hands.first;
        // Instant hand position scaled to screen
        Vec2f hand_pos_inst ;
        
        hand_pos_inst.x = ( hand.get_unit_pos().x - 0.5 ) * cinder::app::getWindowWidth()*2.0 + cinder::app::getWindowWidth()/2.0;
        hand_pos_inst.y = (hand.get_unit_pos().y -0.5)    *   cinder::app::getWindowHeight()*2.0 + cinder::app::getWindowHeight()/2.0;
        
        
        // Filter hand
        hand_pos_f += ( hand_pos_inst - hand_pos_f ) * 0.1 ;
        
        //Pan becomes a grab
        if(hand.get_gesture() == GEST_CLOSE)
        {
            if (track_mode!=MODE_PAN)
            {
                //Grab, record the current maporigin pos
                // and the and pos
                grab_reference = hand_pos_f;
                screen_reference = maporigin;
                std::cout << "Grabbed in " << grab_reference << std::endl;

            }
            track_mode = MODE_PAN;
            
            
            float mapW = cinder::app::getWindowWidth();
            float mapH = cinder::app::getWindowHeight();
            
            float panBorderx = 0.4;
            float panBordery = 0.45;

            maporigin = screen_reference + (hand_pos_f - grab_reference);
            
            
//            if(hand_pos_f.x > mapW*(1.0-panBorderx)) 	maporigin.x -= 5.0;
//            if(hand_pos_f.y  > (mapH*(1.0-panBordery))) 	maporigin.y -= 5.0;//( mapH * panStep - pany ) * 0.05 ;
//            if(hand_pos_f.x  < mapW*panBorderx) 			maporigin.x += 5.0;//( (-mapW * panStep) - panx ) * 0.05 ;
//            if(hand_pos_f.y  < (mapH*panBordery)) 			maporigin.y += 5.0;// ( (-mapH * panStep) - pany ) * 0.05 ;
//            
            
            
            
            
        }
        else{
            track_mode = MODE_CURSOR;

        }
    }
    /// Manage ZOOM slider
    
    
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
        it->update();
    
    gestoos::nui::Hand hand1 = hands.first;
    gestoos::nui::Hand hand2 = hands.second;
    
    // start slider
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
    {
        
        if (!it->is_showing() &&
            hand1.is_present() && hand2.is_present() &&
            std::abs(hand1.get_pos().y - hand2.get_pos().y) < 4 )
        {
            if( hand1.get_gesture() == it->get_trigger()  )
            {
                
                
                
                track_mode = MODE_ZOOM;
                hand_g =        &hand1;
                hand_slider =   &hand2;
                ref_pos =       hand_g->get_pos();
                hide_all_sliders();
                it->show();
                zoomtimer.start();
                offset_zoom = zoom - 0.5;
                it->set_pctg( zoom );
    
                break;
            }
            if(  hand2.get_gesture() == it->get_trigger() )
            {
                track_mode = MODE_ZOOM;

                hand_g =        &hand2;
                hand_slider =   &hand1;
                ref_pos =       hand_g->get_pos();
                hide_all_sliders();
                it->show();
                
                zoomtimer.start();
                offset_zoom = zoom - 0.5;
                it->set_pctg( zoom );
   
                break;
            }
            
        }
    }
    

    // exit slider
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
    {
        
        if( it->is_showing() &&
           it->get_trigger() > 10 && ( !hand1.is_present() || !hand2.is_present()  || hand_g->get_gesture()!=it->get_trigger() ) )
        {
            track_mode = MODE_IDLE;

            hide_all_sliders();
            hand_g =empty_hand;
            hand_slider = empty_hand2;
            std::cout<<"quitting "<<it->get_trigger()<<std::endl;
            break;
        }
    }
    
    // filter ref pos
    if( hand_g->is_present() )
        ref_pos += (hand_g->get_pos() - ref_pos) * 0.1;
    
    // update pctg
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
    {
        if( it->is_showing() && hand_slider->is_present() )
        {
            float new_pctg = 0.0;
            
            if( it->get_type() == Slider::VERTICAL )
            {
                new_pctg = lmap<float>( ( hand_slider->get_pos().y - ref_pos.y ), -20, 20, 1.0, 0.0 );
            }
            
            
           
            it->set_pctg( (new_pctg + offset_zoom)  ) ;
            break;
        }
    }
    
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
    {
        if(track_mode == MODE_ZOOM  && it->is_showing())
        {
            float delta_zoom=zoom;
            zoom =  it->get_pctg();
            delta_zoom = zoom-delta_zoom;
            maporigin.x -=delta_zoom*5;
            maporigin.y -=delta_zoom*5;
            
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
    
    Rectf mpos(0.0,0.0,image.getWidth(),image.getHeight());
    


    mpos.offsetCenterTo(Vec2f(maporigin.x,maporigin.y));
    mpos.scale(1.0 + (zoom - 0.5));
    
   
    gl::draw(image,mpos );
    
    
    if(  hands.first.is_present())
    {
        Vec2f hpos ;
        hpos.x = hand_pos_f.x;
        hpos.y = hand_pos_f.y; // horizontal
        
        if(track_mode == MODE_PAN)
        {
            gl::color( ColorA( 0.6,0.7,0.8,0.6));

            gl::drawSolidCircle( hpos, 20.0, 32 );
        }
        else if(track_mode == MODE_CURSOR)
        {
            gl::color( ColorA( 0.6,0.0,0.0,0.6));

            gl::drawSolidCircle( hpos, 20.0, 32 );
        }
        else if (track_mode == MODE_ZOOM)
        {
            gl::color( ColorA( 0.6,0.7,0.8,0.6));
            Rectf rect(0, 0, 40, 40);
            rect.offsetCenterTo(hpos);
            gl::drawSolidRoundedRect(rect, 6);
        }
        
        
    }
    
    
    
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
        it->draw();

    
}



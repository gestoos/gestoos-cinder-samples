//
//  mapTile.h
//  mapView
//
//  Created by Marcel Alcoverro on 27/05/15.
//
//

#ifndef __mapView__mapTile__
#define __mapView__mapTile__

#include "cinder/Cinder.h"
#include "cinder/Rand.h"
#include "cinder/app/AppNative.h"
#include "cinder/Text.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

#include "Cinderactor.h"
#include "MovingRect.h"

#include "Slider.h"


#include <list>

using namespace ci;
using namespace ci::app;


/// \brief Class implementing an active edge representation
class MapTile {
    
public:
    
    typedef MovingRect Widget;
    
    /// Empty constructor
    MapTile();
    
    /// Constructor with active edge height
    //MapTile(float _h);
    
    void init();
    
    
    /// Default destructor
    virtual ~MapTile();
    
    
    
    /// NOT USED. Set the cursor position... for debug.
    void set_cursor_pos( const Vec2f & p );
    
    /// Update
    void update(const std::pair<gestoos::nui::Hand, gestoos::nui::Hand> & h);
    
    /// Draw
    void draw() const;
    
    
    
    
    void hide_all_sliders();
    

    
    
private:
    
    
    ColorA          color;
    float           alpha;
    Cinderactor::StrokeType      sense;
    
    std::pair<gestoos::nui::Hand, gestoos::nui::Hand>  hands;
    Vec2f hand_pos_f;
    
    
    Vec2f maporigin;
    Vec2f grab_reference;
    Vec2f screen_reference;
    
    //Zoom reference, initial distance between hands
    float zoom_ref;
    float distance_ref;
    
    
    Timer timer;
    
    Timer zoomtimer;
    
    
    int currW;
    
    bool snapmode;
    
    gl::Texture image;
    
    int track_mode;
    
    float zoom, offset_zoom;
    
    
    
    Slider slider_ver, slider_hor;
    
    std::list<Slider> sliders;
    
    gestoos::nui::Hand * empty_hand, * empty_hand2;

    
    gestoos::nui::Hand * hand_g;
    gestoos::nui::Hand * hand_slider;
    cv::Point3f ref_pos;




    
};













#endif /* defined(__mapView__mapTile__) */





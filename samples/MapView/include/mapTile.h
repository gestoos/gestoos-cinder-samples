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
    MapTile(float _h);
    
    /// Default destructor
    virtual ~MapTile();
    
    
    /// Signal the active edge to hide
    void hide();
    
    /// Clear the canvas removing all the widgets
    void clear_canvas();
    
    /// Set the current hand detection
    void set_hands( const std::pair<gestoos::nui::Hand, gestoos::nui::Hand> & hands );
    
    /// NOT USED. Set the cursor position... for debug.
    void set_cursor_pos( const Vec2f & p );
    
    /// Update the current active edge
    void update();
    
    /// Draw the active edge
    void draw() const;
    
    /// Is the active edge being shown or not?
    bool is_showing() const;
    
    /// change snap or cursor mode
    void change_mode();
    
    
    void hide_all_sliders();
    

    
    
private:
    
    std::list< Widget > widgets;
    std::list< Widget > canvas_widgets;
    
    ColorA          color;
    Rectf           rect;
    float           alpha;
    Cinderactor::StrokeType      sense;
    
    std::pair<gestoos::nui::Hand, gestoos::nui::Hand>  hands;
    Vec2f hand_pos_f;
    
    bool showing ;
    
    float show_y, hide_y;
    float show_a, hide_a;
    
    Vec2f show_c, hide_c, center;
    
    Vec2f maporigin;
    
    
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





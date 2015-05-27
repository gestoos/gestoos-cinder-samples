//
//  Slider.h
//  Slider
//
//  Created by Mac on 26/05/15.
//
//

#ifndef Slider_Slider_h
#define Slider_Slider_h

#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"   
#include "cinder/Text.h"
#include "cinder/gl/gl.h"

#include  <algorithm>


using namespace ci;
using namespace ci::app;

/// \brief Class implementing an Slider representation
class Slider : public Rectf {
    
public:
    
    /// Empty constructor
    Slider();
    
    /// Constructor with Slider position and size
    Slider(const Vec2f & _p, const Vec2f & _s, const ColorA & _c);
    
    /// Default destructor
    virtual ~Slider();
    
    /// Signal the slider to show
    void show();
    
    /// Signal the slider to hide
    void hide();
    
    bool is_showing() const;
    
    /// Set the slider percentage
    void set_pctg(float _p);
    
    /// Get the slider percentage
    float get_pctg() const;
    
    /// Update the slider, mainly for animations and viz purposes
    void update();
    
    /// Draw the Slider
    void draw( int sense = 0 ) const;
    
private:
    
    float pctg;
    
    ColorA          color;
    
    Vec2f pos_f;
    float alpha, alpha_dest;
    
    bool showing ;
    
    Timer timer;
};


#endif

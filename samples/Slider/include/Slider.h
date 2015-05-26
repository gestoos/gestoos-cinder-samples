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

#include "Cinderactor.h"

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
    
    void show();
    
    void hide();
    
    bool is_showing() const;
    
    /// Set the slider percentage
    void set_pctg(float _p);
    
    /// Get the slider percentage
    float get_pctg() const;
    
    /// Draw the Slider
    void draw( int sense = 0 ) const;
    
private:
    
    float pctg;
    
    ColorA          color;
   // Rectf           rect;
    
    gestoos::nui::Hand hand;
    Vec2f pos_f;
    
    bool showing ;
    
    Timer timer;
};


#endif

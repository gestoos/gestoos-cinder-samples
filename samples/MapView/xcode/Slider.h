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
    
    enum Type {HORIZONTAL, VERTICAL, CIRCULAR};
    
    /// Empty constructor
    Slider();
    
    /// Constructor with Slider position and size
    Slider(const Vec2f & _p, const Vec2f & _s, const ColorA & _c, int _t = GEST_VICTORY, Type _tp = VERTICAL );
    
    /// Default destructor
    virtual ~Slider();
    
    /// Signal the slider to show
    void show();
    
    /// Signal the slider to hide
    void hide();
    
    /// Is the Slider being shown?
    bool is_showing() const;
    
    /// Set the slider percentage
    void set_pctg(float _p);
    
    /// Get the slider percentage
    float get_pctg() const;
    
    /// Set the slider trigger gesture
    void set_trigger( int _t );
    
    /// Get the slider trigger gesture
    int get_trigger() const;
    
    /// Set the slider type
    void set_type( Type _t );
    
    /// Get the slider type
    Type get_type() const;

    
    /// Update the slider, mainly for animations and viz purposes
    void update();
    
    /// Draw the Slider
    void draw() const;
    
private:
    
    float pctg;
    
    // the gesture trigger
    int trigger;
    
    Type type;
    
    ColorA          color;
    
    Vec2f pos_f;
    float alpha, alpha_dest;
    
    bool showing ;
    
    Timer timer;
    
    
    void _drawArc(Vec2f center, int width, int height, float start, float stop, int lw = 1.0, int res = 64) const;
    

};


#endif

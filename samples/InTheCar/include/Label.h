//
//  Label.h
//  InTheCar
//
//  Created by Mac on 21/05/15.
//
//

#ifndef InTheCar_Label_h
#define InTheCar_Label_h

#include "cinder/Cinder.h"
#include "cinder/Rand.h"
#include "cinder/app/AppNative.h"
#include "cinder/Text.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

#include "Cinderactor.h"
#include "CinderDrive.h"
	
using namespace ci;
using namespace ci::app;



class Label : public TextBox{

public:


    
    
    Label();
    
    Label(const std::string & _t, float _w = 200, CinderDrive::StrokeType _sense = CinderDrive::RIGHT);
       
    virtual ~Label();
    
    void set_sense( CinderDrive::StrokeType _s );
    
    void signal_kill();
    
    bool is_dead() const;
    
    void update();
    
    void draw() const;
    
private:
    
    gl::Texture		mTextTexture;
    Font			mFont;
    ColorA          color;
    Vec2f           position;
    float           alpha;
    bool            dead;
    int             idnum;
    CinderDrive::StrokeType      sense;

    float X_TARGET_RIGHT ;
    float X_TARGET_LEFT ;
    float Y_TARGET_UP ;
    float TARGET_ALPHA ;

};

#endif

//
//  Help.h
//  Slider
//
//  Created by Mac on 09/06/15.
//
//

#ifndef Slider_Help_h
#define Slider_Help_h

#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"

#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/ip/Resize.h"
#include "cinder/ImageIo.h"


using namespace ci;
using namespace ci::app;

class GestoosHelp {
    
public:
    GestoosHelp( app::WindowRef window, const cinder::DataSourceRef & _image )
                : mWindow( window )
    {
         image = gl::Texture( loadImage( _image ) );
        
        _visible = true;
        _y = -getWindowHeight();
        
        mCbKeyDown = mWindow->getSignalKeyDown().connect( std::bind( &GestoosHelp::keyDown, this, std::placeholders::_1 ) );

    }
    ~GestoosHelp(){}

    void setImage(const cinder::DataSourceRef & _image )
    {
        image = gl::Texture( loadImage( _image ) );
    }

    void toggleVisible()
    {
        _visible = !_visible;
    }
    
    void update()
    {
        if( _visible )
        {
            _y += ( 0 - _y ) * 0.1;
        }
        else
        {
            _y += ( -getWindowHeight() - _y ) * 0.1;
        }
    }
    
    void draw() const
    {
        //if( _visible )
        {
            gl::draw(image, Rectf( 0, _y, getWindowWidth(), _y+getWindowHeight() ) );
        }
    }
    
    
    void keyDown	(	KeyEvent 	event	)
    {
        if( event.getCode() == KeyEvent::KEY_h ) toggleVisible();
    }
    
    
private:
    gl::Texture image;
    bool _visible;
    
    float _y;
    
    ci::app::WindowRef				mWindow;
	ci::signals::scoped_connection	mCbKeyDown;
};


#endif

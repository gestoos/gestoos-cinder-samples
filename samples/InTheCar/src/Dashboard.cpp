//
//  Dashboard.cpp
//  InTheCar
//
//  Created by Adolfo López Méndez on 04/06/15.
//
//

#include "Dashboard.h"

#include "cinder/app/AppBasic.h"


#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/ip/Resize.h"

using namespace ci;
using namespace ci::app;


Dashboard::Screen::Screen()
{
    
}


Dashboard::Screen::Screen(const cinder::DataSourceRef & image , const Vec2f & origin, const Vec2f & size )
{
    _src_img =   Surface( loadImage(image) );
    _src_size = Vec2f(_src_img.getWidth(), _src_img.getHeight() );
    
    //Create the final image
    _init_image(_src_img, origin, size);
    _origin=origin;
}

void
Dashboard::Screen::draw()
{
    gl::draw(_img, _origin );
}

void
Dashboard::Screen::update()
{
    
}


void Dashboard::Screen::_init_image( const Surface & image,  const Vec2f & origin,  const Vec2f & size )
{
    if( size.x < 1 )
    {
        _img = image;
        Area srcArea( 0, 0, image.getWidth(), image.getHeight() );
        Surface resized( getWindowSize().x, app::getWindowSize().y, true );
        ip::resize(_img, srcArea, &resized, Area(0, 0,  getWindowSize().x, app::getWindowSize().y), FilterCubic());
        _img = resized.clone();
    }
    else
    {
        std::cout<<" creating image "<< size <<std::endl;

        Area srcArea( 0, 0, image.getWidth(), image.getHeight() );
        Surface resized( size.x, size.y, true );
        ip::resize(image, srcArea, &resized, Area(0,0,size.x, size.y), FilterCubic());
        _img = resized.clone();
    }
    
}








Dashboard::Dashboard()
{
    
}



Dashboard::~Dashboard()
{
    _origin=Vec2f(0, 0);
   _size=Vec2f(getWindowSize().x, getWindowSize().y);
    _current_screen=0;
}

Dashboard::Dashboard(const ci::Vec2f & origin, const ci::Vec2f & size)
{
    this->init(origin, size);
}

void Dashboard::init(const ci::Vec2f & origin, const ci::Vec2f & size)
{
    _origin=origin;
    _size=size;
    _current_screen=0;
}


    
void 
Dashboard::add_screen(const cinder::DataSourceRef & _image )
{
    _screens.push_back(Screen(_image, _origin, _size));
    
}
    
void 
Dashboard::draw()
{
    _screens[_current_screen].draw();
}
    
void 
Dashboard::update()
{
    
}
    
void 
Dashboard::next()
{
    _current_screen=(_current_screen+1)%_screens.size();
}
    


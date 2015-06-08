//
//  Dashboard.h
//  InTheCar
//
//  Created by Adolfo López Méndez on 04/06/15.
//
//

#ifndef __InTheCar__Dashboard__
#define __InTheCar__Dashboard__

#include <iostream>


#include "cinder/Surface.h"
#include "cinder/ImageIo.h"


class Dashboard
{
public:
    
    class Screen
    {
    public:
        Screen();
        
        Screen(const cinder::DataSourceRef & image , const ci::Vec2f & origin, const ci::Vec2f & size = ci::Vec2f(0,0));
        
        void draw();
        
        void update();
        
        //TODO: Operator=?
        
    private:
        ci::Surface _img;
        ci::Surface _src_img;
        ci::Vec2f _size;
        ci::Vec2f _src_size;
        ci::Vec2f _origin;
        cinder::DataSourceRef im_ref_p;
        void _init_image( const ci::Surface & image,  const ci::Vec2f & _o,  const ci::Vec2f & _s );
    };
    
    Dashboard();
    
    Dashboard(const ci::Vec2f & origin, const ci::Vec2f & size = ci::Vec2f(0,0));
    
    void init(const ci::Vec2f & origin, const ci::Vec2f & size = ci::Vec2f(0,0));
    
    ~Dashboard();
    
    void add_screen(const cinder::DataSourceRef & _image);
    
    void draw();
    
    void update();
    
    void next();
    
private:
    ci::Vec2f _origin;
    ci::Vec2f _size;
    
    std::deque<Screen> _screens;
    int _current_screen;
    
};
#endif /* defined(__InTheCar__Dashboard__) */

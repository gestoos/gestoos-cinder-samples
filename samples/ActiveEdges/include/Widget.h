//
//  Widget.h
//  ActiveEdge
//
//  Created by Mac on 28/05/15.
//
//

#ifndef ActiveEdge_Widget_h
#define ActiveEdge_Widget_h

#include "MovingRect.h"

#include "cinder/app/AppBasic.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/ip/Resize.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

class Widget : public MovingRect {
public:
    
    Widget() : MovingRect()
    {
        fixed_alpha = false;
    }
    
    
    Widget( const cinder::DataSourceRef & _image , const Vec2f & _o,  Vec2f _s = Vec2f(0,0), int _i = 0 ) : MovingRect( _o, _s, ColorA(1.0,1.0,1.0,1.0) )
    {
        fixed_alpha = false;
        
        orig_image = Surface( loadImage(_image) );
        orig_size = Vec2f(orig_image.getWidth(), orig_image.getHeight() );
        
        _init_image( orig_image, _o, _s);
        
        num = _i;
        
        hide();
        //        if( _s.x < 1 )
        //        {
        //            image = orig_image;
        //            this->set( _o.x, _o.y, _o.x + orig_image.getWidth(), _o.y + orig_image.getHeight() );
        //            orig_size = Vec2f(orig_image.getWidth(), orig_image.getHeight() );
        //        }
        //        else
        //        {
        //            Surface input( orig_image );
        //            orig_size = Vec2f(orig_image.getWidth(), orig_image.getHeight() );
        //
        //            Area srcArea( 0, 0, input.getWidth(), input.getHeight() );
        //            Surface resized( _s.x, _s.y, true );
        //            ip::resize(input, srcArea, &resized, Area(0,0,_s.x, _s.y), FilterCubic());
        //            image = gl::Texture( resized );
        //        }
        
    }
    
    Widget( const Surface & _image , const Vec2f & _o,  Vec2f _s = Vec2f(0,0), int _i = 0 ) : MovingRect( _o, _s, ColorA(1.0,1.0,1.0,1.0) )
    {
        fixed_alpha = false;
        orig_image = _image.clone();
        orig_size = Vec2f(orig_image.getWidth(), orig_image.getHeight() );
        num = _i;
        _init_image( orig_image, _o, _s);
        //        if( _s.x < 1 )
        //        {
        //            image = orig_image;
        //            this->set( _o.x, _o.y, _o.x + orig_image.getWidth(), _o.y + orig_image.getHeight() );
        //        }
        //        else
        //        {
        //            Surface input( orig_image );
        //            Area srcArea( 0, 0, input.getWidth(), input.getHeight() );
        //            Surface resized( _s.x, _s.y, true );
        //            ip::resize(input, srcArea, &resized, Area(0,0,_s.x, _s.y), FilterCubic());
        //            image = gl::Texture( resized );
        //        }
    }
    
    Widget( const Widget & other ) : MovingRect( Vec2f(other.getX1(), other.getY1()), Vec2f(other.getWidth(), other.getHeight()), ColorA(1.0,1.0,1.0,1.0) )
    {
        orig_image = other.orig_image.clone();
        image = other.image.clone();
    }
    
    const Surface & get_image() const
    {
        return image;
    }
    
    void update()
    {
        MovingRect::update();
        
        fade += (1.0 - fade) * 0.05;
    }
    
    
    void draw() const
    {
        //        std::cout<<"alpha "<<alpha<<std::endl;
        
        if( fixed_alpha )
        {
            gl::color(1.0*fade, 1.0*fade, 1.0, 0.8);
        }
        else
        {
            if( hover || highlight )
            {
                gl::color(1.0, 1.0, 1.0, 1.0 * alpha );
                Rectf border(0, 0, 120, 120);
                border.offsetCenterTo(this->getCenter());
                gl::drawSolidRoundedRect(border, 5);
            }
            else
                gl::color(1.0, 1.0, 1.0, 0.15 * alpha );
        }
        gl::draw(image, Vec2f( getX1(), getY1() ) );
        //cinder::gl::drawSolidRect( *this );
        gl::color(1.0,1.0, 1.0, 1.0);

    }
    
    void heartbeat()
    {
        fade = 0.2;
    }
    
    Surface image;
    Surface orig_image;
    
    cinder::DataSourceRef im_ref_p;
    Vec2f orig_size;
    
    bool fixed_alpha;
    int num;
    
    float fade;
private:
    
    void _init_image( const Surface & _image,  const Vec2f & _o,  const Vec2f & _s )
    {
        if( _s.x < 1 )
        {
            image = _image;
            this->set( 0, 0, _image.getWidth(), _image.getHeight() );
            this->offsetCenterTo( _o );
        }
        else
        {
            std::cout<<" creating image "<<_s<<std::endl;
            //Surface input( _image );
            Area srcArea( 0, 0, _image.getWidth(), _image.getHeight() );
            Surface resized( _s.x, _s.y, true );
            ip::resize(_image, srcArea, &resized, Area(0,0,_s.x, _s.y), FilterCubic());
            image = resized.clone();
        }
        
    }
    
};

#endif

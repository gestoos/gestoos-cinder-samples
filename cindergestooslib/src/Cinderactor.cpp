//
//  Cinderactor.cpp
//  ActiveEdge
//
//  Created by Mac on 21/05/15.
//
//

#include "Cinderactor.h"


Cinderactor::Cinderactor() 
{
    block_timer.start();
    init_ok = false;
}

Cinderactor::~Cinderactor(){}

void Cinderactor::init(const std::string & ini_file )
{
    // Get a reference to the main bundle
#ifdef _WIN32
	std::string bundle_path(".\\resources\\");
#else
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyBundleURL(mainBundle);
	CFStringRef str = CFURLCopyFileSystemPath( resourcesURL, kCFURLPOSIXPathStyle );
	CFRelease(resourcesURL);
	char path[PATH_MAX];
	
	CFStringGetCString( str, path, FILENAME_MAX, kCFStringEncodingASCII );
	CFRelease(str);
    std::string bundle_path(path);
    bundle_path= bundle_path+"/Contents/Resources/";
#endif
    std::cout << ">>>>>>>>>>>> BUNDLE : bundle_path " << bundle_path << std::endl;
    gestoos::nui::Interactor::set_resources_path(bundle_path);
    gestoos::nui::Interactor::init( ini_file );
    init_ok = true;
}

Cinderactor::StrokeType Cinderactor::detect_hand_stroke( int gest, float timeout )
{
    if( get_hands().first.is_present() )
    {
        int hand_gesture = get_hands().first.get_gesture();
        Vec2f hand_vel = Vec2f( get_hands().first.get_vel().x , get_hands().first.get_vel().y );
        
        if( (hand_gesture == gest || gest == -1 ) && block_timer.getSeconds() >= timeout )
        {
            if( hand_vel.y < -6.0 && std::fabs( hand_vel.x ) < 4.0  )
            {
                block_timer.start();
                return UP;
            }
            else if( hand_vel.y > 6.0 && std::fabs( hand_vel.x ) < 4.0  )
            {
                block_timer.start();
                return DOWN;
            }
            else if( hand_vel.x > 6.0 && std::fabs( hand_vel.y ) < 4.0)
            {
                block_timer.start();
                return RIGHT;
            }
            else if( hand_vel.x < -6.0 && std::fabs( hand_vel.y ) < 4.0 )
            {
                block_timer.start();
                return LEFT;
            }
        }
    }
    
    return NONE;
}

void Cinderactor::draw() const
{
    // Loading message
    if( !init_ok )
    {
        gl::color( ColorA( 0.0,0.0,0.0,0.5));
        Rectf rect(0, 0, 300, 100);
        rect.offsetCenterTo(ci::app::getWindowCenter());
        gl::drawSolidRoundedRect(rect, 10);
        gl::drawStringCentered(	"Loading models... please wait...", ci::app::getWindowCenter() );
        gl::color(1.0,1.0, 1.0, 1.0);
        return;
    }

    draw_hand_representation( get_hands().first,   Vec2f(200, 70 ) );
    draw_hand_representation( get_hands().second,  Vec2f(70,  70 ) );
   
       std::stringstream ss;
        ss<<"Static : "<<get_gesture().id <<std::endl;
        gl::drawStringCentered(	ss.str(), Vec2f( 330, 30), Color(0.7, 0.8, 0.9) );
    
}

void Cinderactor::draw_hand_representation( const gestoos::nui::Hand & hand, const Vec2f & where )
{
    float scale = 3.0;
    
    if( hand.is_present() )
    {
        if( hand.get_gesture() == GEST_VICTORY )
            gl::color( Color(1.0, 0.6, 0.7));
        else if( hand.get_gesture() == GEST_EL )
            gl::color( Color(0.5, 1.0, 0.7));
        else
            gl::color( Color(0.5, 0.6, 0.7));
        
        gl::lineWidth(1.0);
        gl::drawStrokedCircle( where, scale*6.0, 32);
        gl::lineWidth(3.0);
        gl::drawLine( where, where + scale*Vec2f( hand.get_vel().x , hand.get_vel().y ) );
        
        std::stringstream ss;
        ss<<"gesture "<<hand.get_gesture();
        gl::drawStringCentered(	ss.str(), where - Vec2f( 0, 40), Color(1.0, 1.0, 1.0) );
    }
    else
    {
        gl::color( Color(0.3, 0.4, 0.5));
        gl::lineWidth(1.0);
        gl::drawStrokedCircle( where, scale*6.0, 32);
        
        gl::drawStringCentered(	"no hand", where - Vec2f( 0, 40), Color(0.3, 0.4, 0.5) );
    }
    
    //Set color back to white
    gl::color( Color(1.0, 1.0, 1.0) );
}

void Cinderactor::draw_hand_circle( const gestoos::nui::Hand & hand ) const
{
    if( hand.is_present() )
    {
        gl::color( Color(0.3, 0.4, 0.5));
        gl::drawStrokedCircle( Vec2f( hand.get_pos().x * cinder::app::getWindowWidth() / 320.0,  hand.get_pos().y * cinder::app::getWindowHeight() / 240.0), 15.0, 32  );
    }
}

bool Cinderactor::get_init_ok()
{
	return init_ok;
}
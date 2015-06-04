//
//  CinderDrive.cpp
//  InTheCar
//
//  Created by Adolfo López Méndez on 28/05/15.
//
//

#include <stdio.h>

#include "CinderDrive.h"

std::deque< cv::Point2f > _pos_queue;
std::deque< cv::Point2f > vel_queue;
std::deque<int> gesture_queue;

CinderDrive::CinderDrive()
{
    block_timer.start();
    init_ok = false;
}

CinderDrive::~CinderDrive(){}

void CinderDrive::init(const std::string & ini_file )
{
    // Get a reference to the main bundle
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyBundleURL(mainBundle);
	CFStringRef str = CFURLCopyFileSystemPath( resourcesURL, kCFURLPOSIXPathStyle );
	CFRelease(resourcesURL);
	char path[PATH_MAX];
	
	CFStringGetCString( str, path, FILENAME_MAX, kCFStringEncodingASCII );
	CFRelease(str);
    std::string bundle_path(path);
    bundle_path= bundle_path+"/Contents/Resources/";
    std::cout << ">>>>>>>>>>>> BUNDLE : bundle_path " << bundle_path << std::endl;
    gestoos::nui::DriverInteraction::set_resources_path(bundle_path);
    
    gestoos::nui::DriverInteraction::init( ini_file );
    
    
    //Load mask resources
    gestoos::nui::DriverInteraction::set_scene_mask(bundle_path +"/InteractionMask.png");
    init_ok = true;
}

float gesture_probability_at_i(int gesture, const std::deque<int> & q, int i)
{
    float p=0;
    float alpha=0.2;
    if (q[i]==gesture)
    {
        p=1.;
        return p;
    }

    if (i > 0)
    {
        if (q[i-1]==gesture) return alpha;
    }
    
    if (i < q.size()-1)
    {
        if (q[i+1]==gesture) return alpha;
    }
    return p;
}


float right_stroke_negative_log_probability_at_i(const std::deque<cv::Point2f> & v, int i)
{

    float lambda_stroke=1.0;
    float lambda_ortho=2.0;
    
    float logp_stroke = lambda_stroke*v[i].x-0.005;
    logp_stroke -= std::abs(lambda_ortho*v[i].y);
    return logp_stroke;
}

float right_stroke_log_p(int gesture, const std::deque<int> & q, const std::deque<cv::Point2f> & v)
{
    float logp=0;
    float pg=1.0;
    for (int i=v.size()-1; i>=0; --i)
    {
        if ((pg=gesture_probability_at_i(gesture, q, i)) > 0)
        {
            logp+=pg*right_stroke_negative_log_probability_at_i(v, i);
        }
    }
    
    return logp;
}

CinderDrive::StrokeType CinderDrive::detect_hand_stroke( int gest, float timeout )
{
    if( get_hand().is_present() )
    {
//        const gestoos::nui::Hand & hand = this->get_hand();
//        //Update positions, velocities and gestures
//        cv::Point2f this_pos= hand.get_unit_pos();
//        _pos_queue.push_back( this_pos );
//        if (_pos_queue.size()==1)
//        {
//            filtered_hand_pos=this_pos;
//        }
//        else if (_pos_queue.size() > 1)
//        {
//            filtered_hand_pos=0.9*filtered_hand_pos + 0.1*get_hand().get_unit_pos();
//        }
//        
//       
//        if (_pos_queue.size() > 5) _pos_queue.pop_front();
//        
//        //Average velocity
//        cv::Point2f velocity(0., 0);
//        if (_pos_queue.size() >= 2)
//        {
//            for (std::size_t i=0; i< _pos_queue.size() -1; ++i)
//            {
//                velocity = velocity + (_pos_queue[i+1]-_pos_queue[i]);
//            }
//            velocity=1./(float)(_pos_queue.size()-1)*velocity;
//            vel_queue.push_back(_pos_queue.back() - _pos_queue[_pos_queue.size()-2]);
//        }
//        
//        if (vel_queue.size() > 5) vel_queue.pop_front();
//       
//
//            
//        int hand_gesture = get_hand().get_gesture();
//        
//        gesture_queue.push_back(hand_gesture);
//        if (gesture_queue.size() > 5) gesture_queue.pop_front();
//        
//        
//        float right_stroke = 0;
//        if (vel_queue.size() >=2)
//        {
//            right_stroke=right_stroke_log_p(GEST_VICTORY, gesture_queue, vel_queue);
//
//            //std::cout << "Right stroke log p " << right_stroke << std::endl;
//            if (right_stroke > 0 && block_timer.getSeconds() >= timeout)
//            {
//                std::cout << "Right stroke log p " << right_stroke << std::endl;
//                block_timer.start();
//                std::cout << "RIGHT STROKE !!!!!!!!!!!!!!!!!!!!! " << std::endl;
//                return RIGHT;
//            }
//        }
//        
        
        
        int hand_gesture = get_hand().get_gesture();
        Vec2f hand_vel = Vec2f( get_hand().get_vel().x , get_hand().get_vel().y );
    
        //std::cout << "Get vel " << get_hand().get_vel() << std::endl;
        
        if( hand_gesture == gest  && block_timer.getSeconds() >= timeout )
        {
            if( hand_vel.x > 0.0 && std::fabs( hand_vel.y ) < 10.0)
            {
                block_timer.start();
                return RIGHT;
            }
            else if( hand_vel.x < -0.0 && std::fabs( hand_vel.y ) < 8.0 )
            {
                block_timer.start();
                return LEFT;
            }
        }
    }
    else{
        if (_pos_queue.size())
        {
            _pos_queue.clear();
            vel_queue.clear();
            gesture_queue.clear();
        }
    }
    
    return NONE;
}

void CinderDrive::draw() const
{
    // Loading message
    if( !init_ok )
    {
        gl::drawStringCentered(	"Loading models... please wait...", Vec2f( cinder::app::getWindowWidth()/2,cinder::app::getWindowHeight()*0.5 ) );
        
        return;
    }
    
    draw_hand_representation( get_hand(),   Vec2f(200, 70 ) );
    
}

void CinderDrive::draw_hand_representation( const gestoos::nui::Hand & hand, const Vec2f & where )
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
    gl::color( Color(1.0, 1.0, 1.0));
    
}

void CinderDrive::draw_hand_circle( const gestoos::nui::Hand & hand ) const
{
    if( hand.is_present() )
    {
        gl::color( Color(0.3, 0.4, 0.5));
        gl::drawStrokedCircle( Vec2f( hand.get_pos().x * cinder::app::getWindowWidth() / 320.0,  hand.get_pos().y * cinder::app::getWindowHeight() / 240.0), 15.0, 32  );
    }
}
#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Thread.h"
//#include "cinder/Text.h"
//#include "cinder/gl/gl.h"
//#include "cinder/gl/Texture.h"
using namespace ci;
using namespace ci::app;

#include <vector>
#include <map>
#include <list>
using namespace std;

//#include "fezoolib/NUI/Interactor.hpp"

#include "Label.h"
#include "AppControl.h"

typedef boost::shared_ptr<Label> LabelPtr;

float MIN_Z_VEL         = 30.0;

// We'll create a new Cinder Application by deriving from the BasicApp class
class exampleApp : public AppNative {
public:
	void	prepareSettings( Settings *settings );
    
	void	mouseDown( MouseEvent event );
	void	mouseDrag( MouseEvent event );
    
	void	touchesBegan( TouchEvent event );
	void	touchesMoved( TouchEvent event );
	void	touchesEnded( TouchEvent event );
    
	void	setup();
    void    update();
	void    draw();
    void    shutdown();
	void	keyDown( KeyEvent event ) { setFullScreen( ! isFullScreen() ); }
    
    void processThread();

    
    
    
    //Cinderactor cinderactor;
    Cinderactor cinderactor;
    AppControl appControl;
    cinder::Timer stroke_timer;
    std::deque< cv::Point2f > pos_queue;

    
    shared_ptr<std::thread>		mThread;
    bool can_process_thread;
    bool init_ok;

    std::list< LabelPtr > labels;
};

void exampleApp::setup()
{
    init_ok = false;
    stroke_timer.start();
    //Start cinderactor processing in a separate thread
    can_process_thread = true;
    mThread = shared_ptr<thread>( new thread( bind( &exampleApp::processThread, this ) ) );
 
    
}

void exampleApp::prepareSettings( Settings *settings )
{
	settings->enableMultiTouch();
    settings->setWindowSize( 800, 600 );
}

void exampleApp::touchesBegan( TouchEvent event )
{
    //console() << "Began: " << event << std::endl;
}

void exampleApp::touchesMoved( TouchEvent event )
{
    //console() << "Moved: " << event << std::endl;
}

void exampleApp::touchesEnded( TouchEvent event )
{
    //console() << "Ended: " << event << std::endl;
}

void exampleApp::mouseDown( MouseEvent event )
{
    //	console() << "Mouse down: " << event.isRight() << " & " << event.isControlDown() << std::endl;
}

void exampleApp::mouseDrag( MouseEvent event )
{
    //	console() << "Mouse drag: " << std::endl;
}

void exampleApp::processThread()
{
   	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread
   
    //Configure the cinderactor
    cinderactor.init( getResourcePath("interactor.cfg").string() );
    cinderactor.set_draw_window(false);
    init_ok = true;
    
    // inifinite processing loop
    while(can_process_thread)
    {
        cinderactor.process();
    }
}

void exampleApp::update()
{
    // Update labels, and delete them if dead
    for (auto it=labels.begin(); it!=labels.end(); ++it) {
        (*it)->update();
        if ((*it)->is_dead()) {
            it = labels.erase(it);
        }
    }
    

        
    // Detect hand strokes
    if (cinderactor.get_hands().first.is_present())
    {
        pos_queue.push_back(cinderactor.get_hands().first.get_unit_pos());
        if (pos_queue.size() > 5) pos_queue.pop_front();
     }
    else
    {
        pos_queue.clear();
    }
    
    cv::Point2f velocity(0., 0);
    if (pos_queue.size() >= 2)
    {
        for (std::size_t i=0; i< pos_queue.size() -1; ++i)
        {
            velocity = velocity + (pos_queue[i+1]-pos_queue[i]);
        }
        velocity=1./(float)(pos_queue.size()-1)*velocity;
    }
    
    // std::cout << "Velocity (unit) " << velocity << " timer " << stroke_timer.getSeconds() << std::endl;
    if ( (float)stroke_timer.getSeconds() > 0.5 && cinderactor.get_hands().first.get_gesture() == GEST_VICTORY )
    {
       
        if (velocity.x > 0.025 && std::abs(velocity.y) < 0.05)
        {
            appControl.simulate_key(AppControl::RIGHT_ARROW);
            stroke_timer.start();
        }
        else if (velocity.x < -0.025 && std::abs(velocity.y) < 0.05)
        {
            appControl.simulate_key(AppControl::LEFT_ARROW);
            stroke_timer.start();
        }
    }
    
//    Cinderactor::StrokeType stroke = cinderactor.detect_hand_stroke( GEST_VICTORY );
    

    // Create labels if stroke detected
//    LabelPtr label_ptr;
//    switch (stroke) {
//            
//        case Cinderactor::RIGHT:
//            if (stroke_timer.getSeconds() > 0.5)
//            {
//                labels.push_back( LabelPtr( new Label("RIGHT", 300, Cinderactor::RIGHT) ) ) ;
//                appControl.simulate_key(AppControl::RIGHT_ARROW);
//                stroke_timer.start();
//            }
//            break;
//            
//        case Cinderactor::LEFT:
//            if (stroke_timer.getSeconds() > 0.5)
//            {
//                labels.push_back( LabelPtr( new Label("LEFT", 300, Cinderactor::LEFT) ) ) ;
//                appControl.simulate_key(AppControl::LEFT_ARROW);
//                stroke_timer.start();
//            }
//            break;
//            
//        default:
//            break;
//    }
}

void exampleApp::draw()
{
	gl::enableAlphaBlending();
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color( 0.1f, 0.13f, 0.16f ) );
    
    // Loading message
    if( !init_ok )
    {
        gl::drawStringCentered(	"Loading models...please wait...", Vec2f( getWindowWidth()/2,getWindowHeight()/2 ) );
        return;
        
    }

    // Draw labels
    for (auto it=labels.begin(); it!=labels.end(); ++it) {
        (*it)->draw();
    }
    
    // Draw cinderactor representation
    cinderactor.draw();
    
}

void exampleApp::shutdown()
{
    can_process_thread = false;
	mThread->join();
    cinderactor.stop();
}

CINDER_APP_NATIVE( exampleApp, RendererGl )
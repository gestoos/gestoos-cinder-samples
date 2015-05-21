#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Thread.h"
using namespace ci;
using namespace ci::app;

#include <vector>
#include <map>
#include <list>
using namespace std;

#include "fezoolib/Core/CaptureRGBD.hpp"
#include "fezoolib/Core/DepthFiltering.hpp"
#include "fezoolib/Tracking/WHAITracker.hpp"


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

    gestoos::CaptureRGBD        capture;
    
    //To store the current frame num
    gestoos::tracking::ObjectTrack::ts_type frame=0;
    
    //Declare WHAITracker
    gestoos::tracking::WHAITracker whai;
    cv::Mat depth_map;
    
   
    shared_ptr<std::thread>		mThread;
    bool can_process_thread;
    
};

void exampleApp::setup()
{
    //Configure camera
    capture.init("",  0,  gestoos::CaptureRGBD::QVGA_30FPS);
    
    //Configure the tracker
    whai.init( getResourcePath("tracker.ini").string() );
    
    //Start interactor processing in a separate thread
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
   
    // inifinite processing loop
    while(can_process_thread)
    {
        /*
         Capture
         */
        capture.get_depth_frame();
        depth_map = capture.depth_frame();
        
        /*
         Filter data, mainly filling depth holes
         */
        depth_map=gestoos::depth_error_filter(depth_map);
        
        /*
         Track hands using WHAI
         */
        whai.update(depth_map, frame);
    }
}

void exampleApp::update()
{
    
}

void exampleApp::draw()
{
	gl::enableAlphaBlending();
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
    
    // Get and draw hands
    std::vector< gestoos::tracking::ObjectTrack *> hands = whai.active_tracks();
    for( auto it=hands.begin(); it!=hands.end(); ++it)
    {
        gl::drawStrokedCircle( Vec2f( (*it)->get_position().x * 800 / 320.0, (*it)->get_position().y * 600 / 240.0), 20.0f );
    }
    
}

void exampleApp::shutdown()
{
    can_process_thread = false;
	mThread->join();
}

CINDER_APP_NATIVE( exampleApp, RendererGl )
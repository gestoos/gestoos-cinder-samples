#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Thread.h"
#include <fezoolib/Core/CaptureRGBD.hpp>

using namespace ci;
using namespace ci::app;

using namespace std;

#include "Cinderactor.h"

#include "mapTile.h"


#include "Help.h"
#include "Resources.h"

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
	void	keyDown( KeyEvent event ) { }
    
    void processThread();
    
    Cinderactor cinderactor;
    
    gestoos::CaptureRGBD capture;
    bool camera_connected;
    cv::Mat depth_copy;
    
    boost::mutex d_mutex;
    
    shared_ptr<std::thread>		mThread;
    bool can_process_thread;
    
    bool init_ok;
    
    MapTile map_tile;

    GestoosHelp *help;

};

void exampleApp::setup()
{
    setFullScreen(true);

    //Start interactor processing in a separate thread
    can_process_thread = true;
    mThread = shared_ptr<thread>( new thread( bind( &exampleApp::processThread, this ) ) );
    
    map_tile.init();
    
    // This installs a Help image with a listener listener which intercepts key-down events
    help = new GestoosHelp( getWindow(), loadResource(IMG_HELP) );
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
    
    camera_connected = capture.init("", gestoos::CaptureRGBD::SensorMode::OPENNI, gestoos::CaptureRGBD::FPS30, gestoos::CaptureRGBD::FrameResolution::QVGA, true); //, true, false);
    capture.set_depth_registration(false);
    
    cout << "Camera conected " << camera_connected << endl;

    
    //Configure the cinderactor
#ifdef _WIN32 // this flag is always defined in Windows 32/64
	cinderactor.init(".\\resources\\interactor.cfg");
#else
    cinderactor.init( getResourcePath("interactor.cfg").string() );
#endif
    cinderactor.set_draw_window(false);
    
    // inifinite processing loop
    while(can_process_thread)
    {
        capture.get_frame();
        
        //Copy frame
        boost::mutex::scoped_lock lock(d_mutex);
        {
            depth_copy = capture.depth_frame().clone();
        }
        
        //Update current app
        if (!depth_copy.empty())
        {
            
            cinderactor.process(depth_copy);
        }
        

    }
}

void exampleApp::update()
{
    help->update();
  
    map_tile.update(cinderactor.get_hands());
}

void exampleApp::draw()
{
	gl::enableAlphaBlending();
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
    
    
    
    map_tile.draw();

    
    cinderactor.draw();
    
    help->draw();
}

void exampleApp::shutdown()
{
    can_process_thread = false;
	mThread->join();
}

CINDER_APP_NATIVE( exampleApp, RendererGl )
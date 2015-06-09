#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Thread.h"
#include <fezoolib/Core/Timestamp.hpp>

using namespace ci;
using namespace ci::app;

using namespace std;

#include "GestureSwitch.h"
#include "Cinderactor.h"


#include "Help.h"
#include "resource.h"

#define GESTURE_TEE 1
#define GESTURE_PAUSE 2
#define GESTURE_VOLDOWN 3
#define GESTURE_VOLUP 4
#define GESTURE_CROSS 5
#define GESTURE_OK 6
#define GESTURE_FWD 7
#define GESTURE_RWD 8


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
	void	keyDown( KeyEvent event ) {  }
    
    void processThread();
    
    Cinderactor cinderactor;
    
    shared_ptr<std::thread>		mThread;
    bool can_process_thread;

	gestoos::detection::GestureDetector::GestureTraits detected_gesture;

	double elapsed = 0;
	int gesture_pos;
	std::vector<GestureSwitch> switches;
    
    GestoosHelp *help;

    
};

void exampleApp::setup()
{

    setFullScreen(true);

	//Load textures
	float switch_size = 200.0;
	Rectf rect(0.0, 0.0, switch_size, switch_size);
	rect.offsetCenterTo(Vec2f(ci::app::getWindowWidth() / 4, ci::app::getWindowHeight() / 3));
	switches.push_back(GestureSwitch(GESTURE_TEE, loadResource(TEE_LOGO), rect));
	rect.offsetCenterTo(Vec2f(ci::app::getWindowWidth() / 4 * 2, ci::app::getWindowHeight() / 3));
	switches.push_back(GestureSwitch(GESTURE_PAUSE, loadResource(PAUSE_LOGO), rect));
	rect.offsetCenterTo(Vec2f(ci::app::getWindowWidth() / 4 * 3, ci::app::getWindowHeight() / 3));
	switches.push_back(GestureSwitch(GESTURE_VOLDOWN, loadResource(VOLDOWN_LOGO), rect));
	rect.offsetCenterTo(Vec2f(ci::app::getWindowWidth() / 4, ci::app::getWindowHeight() / 3 * 2));
	switches.push_back(GestureSwitch(GESTURE_VOLUP, loadResource(VOLUP_LOGO), rect));
	rect.offsetCenterTo(Vec2f(ci::app::getWindowWidth() / 4 * 2, ci::app::getWindowHeight() / 3 * 2));
	switches.push_back(GestureSwitch(GESTURE_FWD, loadResource(FWD_LOGO), rect));
	rect.offsetCenterTo(Vec2f(ci::app::getWindowWidth() / 4 * 3, ci::app::getWindowHeight() / 3 * 2));
	switches.push_back(GestureSwitch(GESTURE_RWD, loadResource(RWD_LOGO), rect));
    
    //Start interactor processing in a separate thread
    can_process_thread = true;
    mThread = shared_ptr<thread>( new thread( bind( &exampleApp::processThread, this ) ) );
    
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
    
    //Configure the cinderactor
#ifdef _WIN32 // this flag is always defined in Windows 32/64
	cinderactor.init("..\\resources\\interactor.cfg");
#else
    cinderactor.init( getResourcePath("interactor_macos.cfg").string() );
#endif
    cinderactor.set_draw_window(false);
    
    // inifinite processing loop
    while(can_process_thread)
    {
		gestoos::Timestamp ts = gestoos::clock_ts();
        cinderactor.process();
		gestoos::Timestamp ts2 = gestoos::clock_ts();

		gestoos::Timestamp diff_ts = ts2 - ts;

		this->elapsed = diff_ts.seconds()*1000.0 + diff_ts.millis();
    }
}

void exampleApp::update()
{
    help->update();

	detected_gesture = cinderactor.get_gesture();
	
	switch (detected_gesture.id) {
	case GESTURE_TEE:
		gesture_pos = 1;
		break;
	case GESTURE_PAUSE:
		gesture_pos = 2;
		break;
	case GESTURE_VOLDOWN:
		gesture_pos = 3;
		break;
	case GESTURE_VOLUP:
		gesture_pos = 4;
		break;
	case GESTURE_FWD:
		gesture_pos = 5;
		break;
	case GESTURE_RWD:
		gesture_pos = 6;
		break;
	default:
		gesture_pos = 0;
		break;
	}
	if (--gesture_pos >= 0)
		switches[gesture_pos].switch_update();
    
}

void exampleApp::draw()
{
	gl::enableAlphaBlending();
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );

	if (cinderactor.get_init_ok()) {
		for (auto it = switches.begin(); it != switches.end(); it++)
			it->draw();
	}

    cinderactor.draw();
    
    help->draw();
}

void exampleApp::shutdown()
{
    can_process_thread = false;
	mThread->join();
}

CINDER_APP_NATIVE( exampleApp, RendererGl )
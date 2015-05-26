#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Thread.h"
#include <fezoolib/Core/Timestamp.hpp>

using namespace ci;
using namespace ci::app;

using namespace std;

#include "Cinderactor.h"

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
	void	keyDown( KeyEvent event ) { setFullScreen( ! isFullScreen() ); }
    
    void processThread();
    
    Cinderactor cinderactor;
    
    shared_ptr<std::thread>		mThread;
    bool can_process_thread;

	gestoos::detection::GestureDetector::GestureTraits detected_gesture;

	double elapsed = 0;
    
};

void exampleApp::setup()
{
    setFullScreen(true);
    
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
    
    //Configure the cinderactor
#ifdef _WIN32 // this flag is always defined in Windows 32/64
	cinderactor.init("..\\resources\\interactor.cfg");
#else
    cinderactor.init( getResourcePath("interactor.cfg").string() );
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

		std::cout << "\r" << this->elapsed << " ms | " << 1 / (this->elapsed / 1000.0) << " fps" << std::flush;
    }
}

void exampleApp::update()
{
	detected_gesture = cinderactor.get_gesture();
    
}

void exampleApp::draw()
{
	gl::enableAlphaBlending();
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );

	std::string elapsedStr = boost::lexical_cast<std::string>(this->elapsed) + " ms | " + boost::lexical_cast<std::string>(1 / (this->elapsed / 1000.0)) + " fps";

	Font font("Arial", 30);
	Color color(1,1,1);
	gl::drawString(elapsedStr, Vec2f(cinder::app::getWindowWidth() / 2, cinder::app::getWindowHeight() / 4),color,font);

	switch (detected_gesture.id) {
	case GESTURE_TEE:
		gl::drawStringCentered("GESTURE TEE", Vec2f(cinder::app::getWindowWidth() / 2, cinder::app::getWindowHeight()*0.5),color,font);
		break;
	case GESTURE_PAUSE:
		gl::drawStringCentered("GESTURE PAUSE", Vec2f(cinder::app::getWindowWidth() / 2, cinder::app::getWindowHeight()*0.5), color, font);
		break;
	case GESTURE_VOLDOWN:
		gl::drawStringCentered("GESTURE SILENCE", Vec2f(cinder::app::getWindowWidth() / 2, cinder::app::getWindowHeight()*0.5), color, font);
		break;
	case GESTURE_VOLUP:
		gl::drawStringCentered("GESTURE AUDIO", Vec2f(cinder::app::getWindowWidth() / 2, cinder::app::getWindowHeight()*0.5), color, font);
		break;
	case GESTURE_FWD:
		gl::drawStringCentered("GESTURE FWD", Vec2f(cinder::app::getWindowWidth() / 2, cinder::app::getWindowHeight()*0.5), color, font);
		break;
	case GESTURE_RWD:
		gl::drawStringCentered("GESTURE RWD", Vec2f(cinder::app::getWindowWidth() / 2, cinder::app::getWindowHeight()*0.5), color, font);
			break;
	default:
		break;

	}
    
    cinderactor.draw();
}

void exampleApp::shutdown()
{
    can_process_thread = false;
	mThread->join();
}

CINDER_APP_NATIVE( exampleApp, RendererGl )
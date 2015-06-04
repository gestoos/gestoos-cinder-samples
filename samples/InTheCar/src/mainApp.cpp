#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Thread.h"
#include "cinder/audio/Voice.h"
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
#include "CinderDrive.h"
#include "Cinderactor.h"

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
	void	keyDown( KeyEvent event ) {  }
    
    void processThread();

    
    
    
    //CinderDrive cinderactor;
    CinderDrive cinderactor;

    
    shared_ptr<std::thread>		mThread;
    bool can_process_thread;
    bool init_ok;

    std::list< LabelPtr > labels;
    //Audio feedback
    audio::VoiceRef mVoice;
    
};

void exampleApp::setup()
{
    init_ok = false;
    //Start cinderactor processing in a separate thread
    can_process_thread = true;
    mThread = shared_ptr<thread>( new thread( bind( &exampleApp::processThread, this ) ) );
    
    //Audio setup
   mVoice = audio::Voice::create( audio::load( loadResource( "correct.wav" ) ) );

 
    
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
    cinderactor.set_draw_window(true);
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
    
    int gesture = cinderactor.detect_hand_gesture(1.0);
    
    switch (gesture)
    {
        case GEST_VICTORY:
        {
            if (mVoice->isPlaying()) mVoice->stop();
            mVoice->start();
            break;
        }
        default:
        {
            break;
        }
    }

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
}

CINDER_APP_NATIVE( exampleApp, RendererGl )
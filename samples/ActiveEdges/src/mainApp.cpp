#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Thread.h"

using namespace ci;
using namespace ci::app;

#include <vector>
using namespace std;

//#include "fezoolib/NUI/Interactor.hpp"

#include "ActiveEdge.h"
#include "Cinderactor.h"


float MIN_Z_VEL         = 30.0;

// We'll create a new Cinder Application by deriving from the BasicApp class
class exampleApp : public AppNative {
public:
	void	prepareSettings( Settings *settings );
    
	void	mouseDown( MouseEvent event );
	void	mouseDrag( MouseEvent event );
    void    mouseMove( MouseEvent event );
  
	void	touchesBegan( TouchEvent event );
	void	touchesMoved( TouchEvent event );
	void	touchesEnded( TouchEvent event );
    void    keyDown( KeyEvent event );

	void	setup();
    void    update();
	void    draw();
    void    shutdown();
    
    void processThread();

    
    Cinderactor cinderactor;

    
    shared_ptr<std::thread>		mThread;
    bool can_process_thread;
    bool init_ok;

    ActiveEdge active_edge;
};

void exampleApp::setup()
{
    setFullScreen(true);
    
    init_ok = false;
    
    active_edge = ActiveEdge(150);
    
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
void exampleApp::mouseMove( MouseEvent event )
{
   // active_edge.set_cursor_pos( event.getPos() );
}

void exampleApp::mouseDrag( MouseEvent event )
{
    //	console() << "Mouse drag: " << std::endl;
}

void exampleApp::keyDown( KeyEvent event )
{
	
		switch( event.getCode() ) {
            case KeyEvent::KEY_s:
                active_edge.show();
                break;
            case KeyEvent::KEY_h:
                active_edge.hide();
                break;
            case KeyEvent::KEY_m:
                active_edge.change_mode();
                break;

		
        }
	
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
    // Detect hand strokes
    Cinderactor::StrokeType stroke = cinderactor.detect_hand_stroke( GEST_VICTORY );
    
    // Activate active_edge if stroke detected
    switch (stroke) {
        case Cinderactor::UP:
            if ( !active_edge.is_showing() )
                active_edge.show();
            break;
            
        case Cinderactor::DOWN:
            if ( active_edge.is_showing() )
                active_edge.hide();
            break;
            
        case Cinderactor::RIGHT:
            if ( !active_edge.is_showing() )
                active_edge.clear_canvas();
            break;

        
        default:
            break;
    }
    
    active_edge.set_hand( cinderactor.get_hands().first );
    active_edge.update();
}

void exampleApp::draw()
{
	gl::enableAlphaBlending();
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color( 0.1f, 0.13f, 0.16f ) );
    
    active_edge.draw();
    
    // Draw cinderactor representation
    cinderactor.draw();
}

void exampleApp::shutdown()
{
    can_process_thread = false;
	mThread->join();
}

CINDER_APP_NATIVE( exampleApp, RendererGl )
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
#include "Widget.h"

#include "resource.h"

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
    
    Rectf r1, r2;
};

void exampleApp::setup()
{
    setFullScreen(true);
    
    init_ok = false;
    
    active_edge = ActiveEdge(200);
    
    
    int pad = getWindowWidth()/5;
    active_edge.add_widget( new Widget( loadResource(L_CHART),      active_edge.rect.getCenter()  + Vec2f(-1.5*pad,   0),  Vec2f(100,100), 0 ) );
    active_edge.add_widget( new Widget( loadResource(L_MAP) ,       active_edge.rect.getCenter()  + Vec2f(-0.5*pad,   0),  Vec2f(100,100), 1 ) );
    active_edge.add_widget( new Widget( loadResource(L_PERSONS),    active_edge.rect.getCenter()  + Vec2f( 0.5*pad,   0),  Vec2f(100,100), 2 ) );
    active_edge.add_widget( new Widget( loadResource(L_ROUND),      active_edge.rect.getCenter()  + Vec2f( 1.5*pad,   0),  Vec2f(100,100), 3 ) );
    

    
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
            case KeyEvent::KEY_a:
                
                Vec2f new_pos = Vec2f( Rand::randInt(200,getWindowWidth()-200), Rand::randInt(200,getWindowHeight()-300) );
                
                r1 = Rectf( new_pos, new_pos + Vec2f(300,300) );
                new_pos = Vec2f( Rand::randInt(200,getWindowWidth()-200), Rand::randInt(200,getWindowHeight()-300) );
                r2 = Rectf( new_pos, new_pos + Vec2f(300,300) );
                
                std::cout<<"INTERSECTS "<<r1<<" | "<<r2<<" -> "<<r1.intersects(r2)<<std::endl;

                break;
                
        }
	
}

void exampleApp::processThread()
{
   	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread
   
    //Configure the cinderactor
#ifdef _WIN32
	cinderactor.init("..\\resources\\interactor.cfg");
#else
    cinderactor.init( getResourcePath("interactor.cfg").string() );
#endif
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
    
    gl::color( ColorA(1.0, 1.0, 1.0, 0.8));
    gl::drawSolidRect( r1);
    gl::drawSolidRect( r2);
    
    // Draw cinderactor representation
    cinderactor.draw();
}

void exampleApp::shutdown()
{
    can_process_thread = false;
	mThread->join();
}

CINDER_APP_NATIVE( exampleApp, RendererGl )
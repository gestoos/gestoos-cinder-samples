#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Thread.h"
using namespace ci;
using namespace ci::app;

using namespace std;

#include "Cinderactor.h"
#include "MovingRect.h"

#include "Help.h"
#include "resource.h"

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
    
    shared_ptr<std::thread>		mThread;
    bool can_process_thread;
    
    gestoos::nui::Hand hand;
    Vec2f hand_pos_f;
    
    MovingRect box;
    Timer timer;
    
    bool grabbing;
    
    GestoosHelp *help;

};

void exampleApp::setup()
{
    setFullScreen(true);
    
    //Start interactor processing in a separate thread
    can_process_thread = true;
    mThread = shared_ptr<thread>( new thread( bind( &exampleApp::processThread, this ) ) );
    
    box = MovingRect( getWindowSize()/2 , Vec2f(200, 200), ColorA(0.3,0.4,0.5,0.5) );
    box.show();
    
    timer.start();
    
    // This installs a Help image with a listener listener which intercepts key-down events
    help = new GestoosHelp( getWindow(), loadResource(IMG_HELP) );
    
    grabbing = false;
    
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
#ifdef _WIN32
	cinderactor.init(".\\resources\\interactor.cfg");
#else
    cinderactor.init( getResourcePath("interactor.cfg").string() );
#endif
    cinderactor.set_draw_window(false);
    
    // inifinite processing loop
    while(can_process_thread)
    {
        cinderactor.process();
    }
}

void exampleApp::update()
{
    
    help->update();
    
    hand = cinderactor.get_hands().first;
    
    
    if( hand.is_present() )
    {
        Vec2f hand_pos_inst ;
        hand_pos_inst.x = hand.get_unit_pos().x * cinder::app::getWindowWidth() ;
        // ( hand.get_pos().x / 320.0 - 0.5 ) * cinder::app::getWindowWidth() *2.0 +   cinder::app::getWindowWidth()/2.0 ;
        hand_pos_inst.y = hand.get_unit_pos().y * cinder::app::getWindowHeight() ;
        //( hand.get_pos().y / 240 - 0.5 ) * cinder::app::getWindowHeight() *2.0 +   cinder::app::getWindowHeight()/2.0 ;
        
        hand_pos_f += (hand_pos_inst - hand_pos_f) * 0.2;
        
        //std::pair<int,int> gr = cinderactor.detect_hand_grabrelease();

        if( hand.get_gesture() == GEST_GRAB &&
           timer.getSeconds() > 0.5 &&
           !grabbing )
        {
            if( box.contains( hand_pos_f ) )
            {
                grabbing = true;
                timer.start();
                std::cout<<"GRAB"<<std::endl;
            }
        }
        else if( hand.get_gesture() == GEST_RELEASE &&
                timer.getSeconds() > 0.2 &&
                grabbing )
        {
            grabbing = false;
            timer.start();
            std::cout<<"DROP"<<std::endl;
        }
        
        if( grabbing )
        {
            box.set_pos( hand_pos_f );
        }
    }
    box.update();
}

void exampleApp::draw()
{
	gl::enableAlphaBlending();
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
    
    box.draw();

    // Draw hand
    if( hand.is_present() )
    {
        if( !grabbing && box.contains(hand_pos_f) )
        {
            gl::color( ColorA(0.6, 0.9, 0.7, 0.5) );
            Rectf rect(0,0,45,45);
            rect.offsetCenterTo( hand_pos_f ) ;
            gl::drawStrokedRoundedRect( rect, 8.0, 32);
        }
        else if( grabbing )
        {
            gl::color( ColorA(0.6, 0.9, 0.7, 0.5) );
            Rectf rect(0,0,45,45);
            rect.offsetCenterTo( hand_pos_f ) ;
            gl::drawSolidRoundedRect( rect, 8.0, 32);
        }
        else
        {
            gl::color( ColorA(0.6, 0.7, 0.8, 0.7) );
            gl::drawStrokedCircle(hand_pos_f, 15, 32);
        }
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
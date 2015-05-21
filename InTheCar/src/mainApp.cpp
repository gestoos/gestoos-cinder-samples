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

#include "fezoolib/NUI/Interactor.hpp"

#include "Label.h"

typedef boost::shared_ptr<Label> LabelPtr;

int GEST_VICTORY        = 14;
float BLOCK_TIMEOUT     = 1.0;
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

    
    
    
    gestoos::nui::Interactor interactor;
    
    std::pair<gestoos::nui::Hand,gestoos::nui::Hand> hands;
    
   
    shared_ptr<std::thread>		mThread;
    bool can_process_thread;
    bool init_ok;
    
    Timer block_timer;
    
    gl::Texture		mTextTexture;
    Vec2f			mSize;
    Font			mFont;

    std::list< LabelPtr > labels;
};

void exampleApp::setup()
{
    init_ok = false;
    
    //Start interactor processing in a separate thread
    can_process_thread = true;
    mThread = shared_ptr<thread>( new thread( bind( &exampleApp::processThread, this ) ) );
    
    block_timer.start();
    
#if defined( CINDER_COCOA )
	mFont = Font( "Arial", 60 );
#else
	mFont = Font( "Times New Roman", 32 );
#endif
	mSize = Vec2f( 100, 100 );
    
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
   
    //Configure the interactor
    interactor.init( getResourcePath("interactor.cfg").string() );
    interactor.set_draw_window(false);
    init_ok = true;
    
    // inifinite processing loop
    while(can_process_thread)
    {
        interactor.process();
    }
}

void exampleApp::update()
{
    
    for (auto it=labels.begin(); it!=labels.end(); ++it) {
        (*it)->update();
        if ((*it)->is_dead()) {
            it = labels.erase(it);
        }
    }
    
    hands = interactor.get_hands();
    
    if( hands.first.is_present() )
    {
        
        Vec2f hand_pos = Vec2f( hands.first.get_pos().x * getWindowWidth() / 320.0 ,
                               hands.first.get_pos().y * getWindowHeight() / 240.0 );
        
        Vec2f hand_vel = Vec2f( hands.first.get_vel().x , hands.first.get_vel().y );
        
        float z_vel = hands.first.get_vel().z ;
        
        int hand_gesture = hands.first.get_gesture();
        int static_gesture = interactor.get_gesture().id;
        
        
        std::string txt;
        if( hand_gesture == GEST_VICTORY && block_timer.getSeconds() > BLOCK_TIMEOUT )
        {
            
            if( hand_vel.x > 6.0 )
            {
                std::cout<<"gest  " <<hand_gesture<<" : vel "<<hand_vel.x<<std::endl;
                
                LabelPtr label_ptr( new Label("RIGHT", 300, Label::RIGHT) );
                labels.push_back( label_ptr ) ;

                block_timer.start();
            }
            else if( hand_vel.x < -6.0 )
            {
                std::cout<<"gest  " <<hand_gesture<<" : vel "<<hand_vel.x<<std::endl;
                
                LabelPtr label_ptr( new Label("LEFT", 300, Label::LEFT) );
                labels.push_back( label_ptr ) ;

                block_timer.start();
            }
            
         }
        else
        {

        }
    }
}

void exampleApp::draw()
{
	gl::enableAlphaBlending();
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color( 0.1f, 0.13f, 0.16f ) );
    
    if( !init_ok )
    {
        gl::drawStringCentered(	"Loading models...please wait...", Vec2f( getWindowWidth()/2, getWindowHeight()/2 ) );
        
    }

    for (auto it=labels.begin(); it!=labels.end(); ++it) {
        (*it)->draw();
    }
    
    hands = interactor.get_hands();
    
    if( hands.first.is_present() )
    {
        std::stringstream ss;
        ss<<"Gest: "<<hands.first.get_gesture();
        gl::drawString(	ss.str(), Vec2f( 30, 30) );
        
        ss.str("");
        ss<<"V.x : "<<hands.first.get_vel().x;
        gl::drawString(	ss.str(), Vec2f( 30, 42) );


    }
    
}

void exampleApp::shutdown()
{
    can_process_thread = false;
	mThread->join();
}

CINDER_APP_NATIVE( exampleApp, RendererGl )
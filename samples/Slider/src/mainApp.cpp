#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Thread.h"
using namespace ci;
using namespace ci::app;

using namespace std;

#include <list>

#include "Cinderactor.h"
#include "Slider.h"


// We'll create a new Cinder Application by deriving from the BasicApp class
class exampleApp : public AppNative {
public:
	void	prepareSettings( Settings *settings );
    
	void	mouseDown( MouseEvent event );
	void	mouseDrag( MouseEvent event );
	void	mouseMove( MouseEvent event );
    
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
    
    gestoos::nui::Hand empty_hand;
    gestoos::nui::Hand * hand_g;
    gestoos::nui::Hand * hand_slider;
    
    cv::Point3f ref_pos;
    Vec2f hand_pos_f;
    
    Slider slider_ver, slider_hor;
    Timer timer;
    
};

void exampleApp::setup()
{
    setFullScreen(true);
    
    //init and hide sliders
    slider_ver = Slider( getWindowSize()/2 , Vec2f(30, 500), ColorA(0.9,0.4,0.5,0.5) );
    slider_ver.hide();
    
    slider_hor = Slider( getWindowSize()/2 , Vec2f(500, 30), ColorA(0.3,0.9,0.5,0.5) );
    slider_hor.hide();
    
    //init pointers
    ref_pos = cv::Point3f(-1,-1,-1);
    empty_hand.clear();
    hand_g = &empty_hand;
    hand_slider = &empty_hand;
    
    timer.start();
    
    
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

void exampleApp::mouseMove( MouseEvent event )
{
    //	console() << "Mouse drag: " << std::endl;
    //slider.set_pctg( event.getPos().y / (float)getWindowHeight() );
}

void exampleApp::processThread()
{
   	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread
    
    //Configure the cinderactor
    cinderactor.init( getResourcePath("interactor.cfg").string() );
    cinderactor.set_draw_window(false);
    
    // inifinite processing loop
    while(can_process_thread)
    {
        cinderactor.process();
    }
}

void exampleApp::update()
{
    slider_hor.update();
    slider_ver.update();
    
    gestoos::nui::Hand hand1 = cinderactor.get_hands().first;
    gestoos::nui::Hand hand2 = cinderactor.get_hands().second;
    
    // start slider
    if( ( hand1.is_present() && hand1.get_gesture() == GEST_VICTORY ) && !slider_ver.is_showing() )
    {
        hand_g = &hand1;
        hand_slider = &hand2;
        ref_pos = hand_g->get_pos();
        slider_ver.show();
        slider_hor.hide();
        std::cout<<"showing ver with hand 1"<<std::endl;
    }
    // start slider
    if( ( hand2.is_present() && hand2.get_gesture() == GEST_VICTORY ) && !slider_ver.is_showing() )
    {
        hand_g = &hand2;
        hand_slider = &hand1;
        ref_pos = hand_g->get_pos();
        slider_ver.show();
        slider_hor.hide();
        std::cout<<"showing ver with hand 2"<<std::endl;
    }
    // start slider
    if( ( hand1.is_present() && hand1.get_gesture() == GEST_EL ) && !slider_hor.is_showing() )
    {
        hand_g = &hand1;
        hand_slider = &hand2;
        ref_pos = hand_g->get_pos();
        slider_hor.show();
        slider_ver.hide();
        std::cout<<"showing hor with hand 1"<<std::endl;
    }
    // start slider
    if( ( hand2.is_present() && hand2.get_gesture() == GEST_EL ) && !slider_hor.is_showing() )
    {
        hand_g = &hand2;
        hand_slider = &hand1;
        ref_pos = hand_g->get_pos();
        slider_hor.show();
        slider_ver.hide();
        std::cout<<"showing hor with hand 2"<<std::endl;
    }
    
    // exit slider
    if( slider_hor.is_showing() && hand_g->get_gesture() != GEST_EL )
    {
        slider_hor.hide();
        hand_g = hand_slider = &empty_hand;
        std::cout<<"quitting hor"<<std::endl;
    }
    // exit slider
    if( slider_ver.is_showing() && hand_g->get_gesture() != GEST_VICTORY )
    {
        slider_ver.hide();
        hand_g = hand_slider = &empty_hand;
        std::cout<<"quitting ver"<<std::endl;
    }
    
    // filter ref pos
    if(slider_ver.is_showing() || slider_hor.is_showing() )
    {
        ref_pos += (hand_g->get_pos() - ref_pos) * 0.1;
    }
    
    // set slider hor
    if( slider_hor.is_showing() && hand_slider->is_present() )
    {
        float new_pctg = 0.0;
        if( hand_slider->get_pos().x > hand_g->get_pos().x )
            new_pctg = lmap<float>( hand_slider->get_pos().x - ref_pos.x,    60,  120, 0.0, 1.0 );
        else
            new_pctg = lmap<float>( hand_slider->get_pos().x - ref_pos.x,  -120,  -60, 0.0, 1.0 );
        
        
        slider_hor.set_pctg( new_pctg ) ;
        //        std::cout<<"moving hor "<<slider_hor.get_pctg()<<std::endl;
    }
    
    // set slider ver
    if( slider_ver.is_showing() && hand_slider->is_present() )
    {
        float new_pctg = lmap<float>( ( hand_slider->get_pos().y - ref_pos.y ), -40, 40, 1.0, 0.0 );
        
        slider_ver.set_pctg( new_pctg ) ;
        //        std::cout<<"moving ver "<<slider_ver.get_pctg() <<std::endl;
    }
}

void exampleApp::draw()
{
	gl::enableAlphaBlending();
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
    
    slider_ver.draw(1);
    slider_hor.draw(0);
    
    cinderactor.draw();
}

void exampleApp::shutdown()
{
    can_process_thread = false;
	mThread->join();
}

CINDER_APP_NATIVE( exampleApp, RendererGl )
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
    
    void hide_all_sliders();
    bool no_widget_showing();
    
    Cinderactor cinderactor;
    
    shared_ptr<std::thread>		mThread;
    bool can_process_thread;
    
    gestoos::nui::Hand empty_hand, static_hand;
    gestoos::nui::Hand * hand_g;
    gestoos::nui::Hand * hand_slider;
    
    cv::Point3f ref_pos;
    Vec2f hand_pos_f;
    
    Slider slider_ver, slider_hor;
    
    std::list<Slider> sliders;
    
    Timer timer;
    
};

void exampleApp::setup()
{
    setFullScreen(true);

    sliders.push_back( Slider( getWindowSize()/2 , Vec2f(30, 500),  ColorA(0.9,0.4,0.5,0.5), GEST_VICTORY,  Slider::VERTICAL )  );
    sliders.push_back( Slider( getWindowSize()/2 , Vec2f(500, 30),  ColorA(0.5,0.9,0.5,0.5), GEST_EL,       Slider::HORIZONTAL )  );
    sliders.push_back( Slider( getWindowSize()/2 , Vec2f(300, 300), ColorA(0.9,0.9,0.5,0.5), GEST_AUDIO,    Slider::CIRCULAR )  );
    
    //init pointers
    ref_pos = cv::Point3f(-1,-1,-1);
    empty_hand.clear();
    static_hand.clear();
    hand_g = hand_slider = &empty_hand;
    
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
    //    slider_hor.update();
    //    slider_ver.update();
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
        it->update();
    
    gestoos::nui::Hand hand1 = cinderactor.get_hands().first;
    gestoos::nui::Hand hand2 = cinderactor.get_hands().second;
    
    // start slider
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
    {
        if( cinderactor.get_gesture().id == it->get_trigger() && no_widget_showing() )
        {
//            hand_g->set_pos( cv::Point3f(cinderactor.get_gesture().u, cinderactor.get_gesture().v, cinderactor.get_gesture().z ) );
//            ref_pos = hand_g->get_pos();
            
            ref_pos = cv::Point3f(cinderactor.get_gesture().u, cinderactor.get_gesture().v, cinderactor.get_gesture().z );
            
            hide_all_sliders();
            it->show();
            std::cout<<"showing static slider "<<it->get_trigger()<<std::endl;
            break;
        }
        if(  hand1.is_present() && hand1.get_gesture() == it->get_trigger()  && no_widget_showing() )
        {
            hand_g =        &hand1;
            hand_slider =   &hand2;
            ref_pos =       hand_g->get_pos();
            hide_all_sliders();
            it->show();
            std::cout<<"showing hand slider "<<it->get_trigger()<<std::endl;
            break;
        }
        if(  hand2.is_present() && hand2.get_gesture() == it->get_trigger()  && no_widget_showing() )
        {
            hand_g =        &hand2;
            hand_slider =   &hand1;
            ref_pos =       hand_g->get_pos();
            hide_all_sliders();
            it->show();
            std::cout<<"showing hand slider "<<it->get_trigger()<<std::endl;
            break;
        }
    }
    
    //Special case, static gesture slider, hand init
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
    {
        if (it->is_showing() &&
            it->get_trigger() < 10 &&
            !hand_slider->is_present() )
        {
            if( hand1.is_present() )
            {
                std::cout<<"setting hand 1 to "<<it->get_trigger()<<std::endl;
                hand_slider = &hand1;
                break;
            }
            if( hand2.is_present() )
            {
                std::cout<<"setting hand 2 to "<<it->get_trigger()<<std::endl;
                hand_slider = &hand2;
                break;
            }
            if( !hand1.is_present() && !hand2.is_present() )
            {
                std::cout<<"removing hand from "<<it->get_trigger()<<std::endl;
                hand_slider = &empty_hand;
                break;
            }
        }
    }
    
    // exit slider
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
    {
        if( it->is_showing() &&
            ( it->get_trigger() < 10 && cinderactor.get_gesture().id != it->get_trigger() ) )
        {
            hide_all_sliders();
            hand_g = hand_slider = &empty_hand;
            std::cout<<"quitting "<<it->get_trigger()<<std::endl;
            break;
        }
        
        if( it->is_showing() &&
           ( it->get_trigger() > 10 && hand_g->get_gesture() != it->get_trigger() ) )
        {
            hide_all_sliders();
            hand_g = hand_slider = &empty_hand;
            std::cout<<"quitting "<<it->get_trigger()<<std::endl;
            break;
        }
    }
    
    // filter ref pos
    if( hand_g->is_present() )
        ref_pos += (hand_g->get_pos() - ref_pos) * 0.1;
    
    // update pctg
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
    {
        if( it->is_showing() && hand_slider->is_present() )
        {
            float new_pctg = 0.0;
            
            if( it->get_type() == Slider::HORIZONTAL )
            {
                if( hand_slider->get_pos().x > hand_g->get_pos().x )
                    new_pctg = lmap<float>( hand_slider->get_pos().x - ref_pos.x,    60,  120, 0.0, 1.0 );
                else
                    new_pctg = lmap<float>( hand_slider->get_pos().x - ref_pos.x,  -120,  -60, 0.0, 1.0 );
            }
            else if( it->get_type() == Slider::VERTICAL )
            {
                new_pctg = lmap<float>( ( hand_slider->get_pos().y - ref_pos.y ), -40, 40, 1.0, 0.0 );
            }
            else if( it->get_type() == Slider::CIRCULAR )
            {
                std::cout<<hand_slider->get_pos().y<<" - "<<ref_pos.y<<std::endl;
                new_pctg = lmap<float>( ( hand_slider->get_pos().y - ref_pos.y ), -10, 60, 1.0, 0.0 );
            }
            
            it->set_pctg( new_pctg ) ;
            break;
        }
    }
    }

void exampleApp::hide_all_sliders()
{
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
        it->hide();
}

bool exampleApp::no_widget_showing()
{
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
        if( it->is_showing() )
        {
            return false;
        }
    return true;
}

void exampleApp::draw()
{
	gl::enableAlphaBlending();
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
    
    for( auto it=sliders.begin(); it!=sliders.end(); ++it )
        it->draw();
    //    slider_ver.draw(1);
    //    slider_hor.draw(0);
    
    cinderactor.draw();
}

void exampleApp::shutdown()
{
    can_process_thread = false;
	mThread->join();
}

CINDER_APP_NATIVE( exampleApp, RendererGl )
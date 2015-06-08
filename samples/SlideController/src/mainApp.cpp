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
#include <thread>
using namespace std;

//#include "fezoolib/NUI/Interactor.hpp"

#include "Label.h"
#include "AppControl.h"

typedef boost::shared_ptr<Label> LabelPtr;

float MIN_Z_VEL         = 30.0;
bool interrupt=false;

// We'll create a new Cinder Application by deriving from the BasicApp class
class exampleApp {
public:
    
	void	setup();
    void    threaded_init();
    void    update();
    void    run();
    void    shutdown();

    
    Cinderactor cinderactor;
    AppControl appControl;
    cinder::Timer stroke_timer;
    std::deque< cv::Point2f > pos_queue;
    cv::Point2f filtered_pos;
    bool mouse_hidden;

    
    shared_ptr<std::thread>		mThread;
    bool can_process_thread;
    bool init_ok;

    std::list< LabelPtr > labels;
};

void exampleApp::setup()
{
    init_ok = false;
    stroke_timer.start();
    mouse_hidden = true;
    //Start cinderactor processing in a separate thread
    can_process_thread = true;
    //mThread = shared_ptr<thread>( new thread( bind( &exampleApp::processThread, this ) ) );
    
    
}

void exampleApp::threaded_init()
{
    // Get a reference to the main bundle
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyBundleURL(mainBundle);
	CFStringRef str = CFURLCopyFileSystemPath( resourcesURL, kCFURLPOSIXPathStyle );
	CFRelease(resourcesURL);
	char path[PATH_MAX];
	
	CFStringGetCString( str, path, FILENAME_MAX, kCFStringEncodingASCII );
	CFRelease(str);
    std::string bundle_path(path);
    bundle_path= bundle_path+"/Contents/Resources/";
    std::cout << ">>>>>>>>>>>> BUNDLE : bundle_path " << bundle_path << std::endl;
    
    //Configure the cinderactor
    cinderactor.init( bundle_path + "/interactor.cfg" );
    cinderactor.set_draw_window(false);
    init_ok = true;

}

void exampleApp::run()
{
    std::thread ti( &exampleApp::threaded_init, this );
    
    while (!init_ok && !interrupt)
    {
        std::cout << "\rWait ..." << std::endl;
        int identifier =
        CFUserNotificationDisplayNotice(
         2,
         kCFUserNotificationNoteAlertLevel,
         NULL,
         NULL,
         NULL,
         CFSTR("SlideController"),
         CFSTR("Loading resources. Wait ..."),
         NULL);
        sleep(2);
    }

    ti.join();
    

    
    CFOptionFlags cfRes;
    
    CFUserNotificationDisplayAlert(0,
                                   kCFUserNotificationNoteAlertLevel,
                                   NULL,
                                   NULL,
                                   NULL,
                                   CFSTR("SlideController: Finished loading resources"),
                                   CFSTR("Click Ok and then open your slide show!"),
                                   CFSTR("OK"),
                                   CFSTR("Quit"),
                                   NULL,
                                   &cfRes);
	
    if (cfRes != kCFUserNotificationDefaultResponse)
    {
           exit(-1);
    }

    
    // inifinite processing loop
    while(can_process_thread)
    {
        cinderactor.process();
        this->update();
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
    

        
    // Detect hand strokes
    if (cinderactor.get_hands().first.is_present())
    {
        pos_queue.push_back(cinderactor.get_hands().first.get_unit_pos());
        if (pos_queue.size()==1) filtered_pos=pos_queue.back();
        else
        {
            if (pos_queue.size() > 5) pos_queue.pop_front();
            filtered_pos = 0.3*pos_queue.back() + 0.7*filtered_pos;
        }
     }
    else
    {
        pos_queue.clear();
    }
    
    cv::Point2f velocity(0., 0);
    if (pos_queue.size() >= 2)
    {
        for (std::size_t i=0; i< pos_queue.size() -1; ++i)
        {
            velocity = velocity + (pos_queue[i+1]-pos_queue[i]);
        }
        velocity=1./(float)(pos_queue.size()-1)*velocity;
    }
    
    // std::cout << "Velocity (unit) " << velocity << " timer " << stroke_timer.getSeconds() << std::endl;
    if ( cinderactor.get_hands().first.get_gesture() ==  GEST_EL)
    {

        int x =1920*filtered_pos.x;
        int y =1080*filtered_pos.y;
        if (mouse_hidden) mouse_hidden=false;
        appControl.simulate_mouse(x, y);
    }
//    else if ( (float)stroke_timer.getSeconds() > 0.5 && cinderactor.get_hands().first.get_gesture() == GEST_RELEASE )
//    {
//        int x = getWindowWidth()*pos_queue.back().x;
//        int y = getWindowHeight()*pos_queue.back().y;
//        appControl.simulate_mouse(x, y, 1, true);
//    }
    else if ( (float)stroke_timer.getSeconds() > 0.5 && cinderactor.get_hands().first.get_gesture() == GEST_VICTORY )
    {
        
       
        if (velocity.x > 0.02 && std::abs(velocity.y) < 0.05)
        {
            std::cout << "RIGHT!" << std::endl;
            appControl.simulate_key(AppControl::RIGHT_ARROW);
            stroke_timer.start();
        }
        else if (velocity.x < -0.02 && std::abs(velocity.y) < 0.05)
        {
            std::cout << "LEFT!" << std::endl;
            appControl.simulate_key(AppControl::LEFT_ARROW);
            stroke_timer.start();
        }
    }
    else
    {
        //Hide mouse
        if (!mouse_hidden)
            appControl.simulate_mouse(2000, 3000);
        mouse_hidden=true;
    }
    
    
//    Cinderactor::StrokeType stroke = cinderactor.detect_hand_stroke( GEST_VICTORY );
    

    // Create labels if stroke detected
//    LabelPtr label_ptr;
//    switch (stroke) {
//            
//        case Cinderactor::RIGHT:
//            if (stroke_timer.getSeconds() > 0.5)
//            {
//                labels.push_back( LabelPtr( new Label("RIGHT", 300, Cinderactor::RIGHT) ) ) ;
//                appControl.simulate_key(AppControl::RIGHT_ARROW);
//                stroke_timer.start();
//            }
//            break;
//            
//        case Cinderactor::LEFT:
//            if (stroke_timer.getSeconds() > 0.5)
//            {
//                labels.push_back( LabelPtr( new Label("LEFT", 300, Cinderactor::LEFT) ) ) ;
//                appControl.simulate_key(AppControl::LEFT_ARROW);
//                stroke_timer.start();
//            }
//            break;
//            
//        default:
//            break;
//    }
}


void exampleApp::shutdown()
{
    can_process_thread = false;
	//mThread->join();
    cinderactor.stop();
}

exampleApp appl;

void catch_signal(int signum)
{
    interrupt=true;
    appl.can_process_thread=false;
}

int main(int argc, char * argv[])
{
    
    // Interrupt
    signal(SIGINT, catch_signal);
    signal(SIGKILL, catch_signal);
    signal(SIGTERM, catch_signal);
    signal(SIGQUIT, catch_signal);
    
    appl.setup();

    if (!interrupt)
        appl.run();

    //TODO: Clean exit
    appl.shutdown();
    
    return 0;
}

#include "cinder/app/AppBasic.h"
#include "Earth.h"
#include "POV.h"
#include "Resources.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Json.h"
#include "cinder/Url.h"
#include "cinder/Vector.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/gl/TileRender.h"
#include "cinder/Thread.h"


using namespace ci;
using namespace ci::app;

#include <vector>
#include <sstream>
using namespace std;
using std::vector;
using std::string;
using std::istringstream;
using std::stringstream;

#include <fezoolib/nui/Interactor.hpp>

int GEST_PAUSE          = 2;
int GEST_L              = 13;
int GEST_VICTORY        = 14;
float BLOCK_TIMEOUT     = 1.0;
float MIN_Z_VEL         = 30.0;

class EarthquakeApp : public AppBasic {
public:
	void prepareSettings( Settings *settings );
	void keyDown( KeyEvent event );
	void mouseMove( MouseEvent event );
	void mouseWheel( MouseEvent event );
	void parseEarthquakes( const string &url );
	void setup();
	void update();
	void draw();
    void shutdown();
    
    void processInteractor();
	
	gl::GlslProg	mEarthShader;
	gl::GlslProg	mQuakeShader;
	
	gl::Texture		mStars;
	
	POV				mPov;
	Earth			mEarth;
	
	Vec2f			mLastMouse;
	Vec2f			mCurrentMouse;
	
	Vec3f			sBillboardUp, sBillboardRight;
	Vec3f			billboardVecs[2];
	
	Vec3f			mLightDir;
	
	float			mCounter;
	int				mCurrentFrame;
	bool			mSaveFrames;
	bool			mShowEarth;
	bool			mShowLand;
	bool			mShowOcean;
	bool			mShowText;
	bool			mShowQuakes;
    
    gestoos::nui::Interactor interactor;
    
    std::pair<gestoos::nui::Hand,gestoos::nui::Hand> hands;

    shared_ptr<std::thread>		mThread;
    bool can_process_interactor;

    Timer block_timer;
    
    float zoom_base;
    int last_hand_gesture;
    
};


void EarthquakeApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1024, 768 );
	settings->setFrameRate( 60.0f );
	settings->setResizable( true );
	settings->setFullScreen( false );
}



void EarthquakeApp::setup()
{

#ifdef _WIN32 // this flag is always defined in Windows 32/64
    
    /// \todo change interactor.cfg to interactorWin.cfg and add to git.
	interactor.init("..\\resources\\interactor.cfg");
#else
    interactor.init(getResourcePath("tracker.ini").string(),
                    getResourcePath("gestures320.ini").string(),
                    getResourcePath("handGestures.ini").string() );
#endif
#
    
    
    //Start interactor processing in a separate thread
    can_process_interactor = true;
    mThread = shared_ptr<thread>( new thread( bind( &EarthquakeApp::processInteractor, this ) ) );

    block_timer.start();
    
    zoom_base = -1;
    last_hand_gesture = -1;
    //-----------------------------------------------
    
	gl::Texture earthDiffuse	= gl::Texture( loadImage( loadResource( RES_EARTHDIFFUSE ) ) );
	gl::Texture earthNormal		= gl::Texture( loadImage( loadResource( RES_EARTHNORMAL ) ) );
	gl::Texture earthMask		= gl::Texture( loadImage( loadResource( RES_EARTHMASK ) ) );
	earthDiffuse.setWrap( GL_REPEAT, GL_REPEAT );
	earthNormal.setWrap( GL_REPEAT, GL_REPEAT );
	earthMask.setWrap( GL_REPEAT, GL_REPEAT );
    
	mStars						= gl::Texture( loadImage( loadResource( RES_STARS_PNG ) ) );
    
	
	mEarthShader = gl::GlslProg( loadResource( RES_PASSTHRU_VERT ), loadResource( RES_EARTH_FRAG ) );
	mQuakeShader = gl::GlslProg( loadResource( RES_QUAKE_VERT ), loadResource( RES_QUAKE_FRAG ) );
    
	
	mCounter		= 0.0f;
	mCurrentFrame	= 0;
	mSaveFrames		= false;
	mShowEarth		= true;
	mShowText		= true;
	mShowQuakes		= true;
	mLightDir		= Vec3f( 0.025f, 0.25f, 1.0f );
	mLightDir.normalize();
	mPov			= POV( this, ci::Vec3f( 0.0f, 0.0f, 1000.0f ), ci::Vec3f( 0.0f, 0.0f, 0.0f ) );
	mEarth			= Earth( earthDiffuse, earthNormal, earthMask );
	
	parseEarthquakes( "http://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/2.5_week.geojson" );
	
	mEarth.setQuakeLocTip();
}

void EarthquakeApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getCode() == app::KeyEvent::KEY_ESCAPE ) {
		setFullScreen( false );
	}
	else if( event.getChar() == 's' ) {
		mSaveFrames = ! mSaveFrames;
	}
	else if( event.getChar() == 'e' ) {
		mShowEarth = ! mShowEarth;
	}
	else if( event.getChar() == 't' ) {
		mShowText = ! mShowText;
	}
	else if( event.getChar() == 'q' ) {
		mShowQuakes = ! mShowQuakes;
	}
	else if( event.getChar() == 'm' ) {
		mEarth.setMinMagToRender( -1.0f );
	}
	else if( event.getChar() == 'M' ) {
		mEarth.setMinMagToRender( 1.0f );
	}
	else if( event.getCode() == app::KeyEvent::KEY_UP ) {
		mPov.adjustDist( -10.0f );
	}
	else if( event.getCode() == app::KeyEvent::KEY_DOWN ) {
		mPov.adjustDist( 10.0f );
	}
	else if( event.getChar() == ' ' ) {
		gl::TileRender tr( 5000, 5000 );
		CameraPersp cam;
		cam.lookAt( mPov.mEye, mPov.mCenter );
		cam.setPerspective( 60.0f, tr.getImageAspectRatio(), 1, 20000 );
		tr.setMatrices( cam );
		while( tr.nextTile() ) {
			draw();
		}
		writeImage( getHomeDirectory() / "output.png", tr.getSurface() );
	}
}


void EarthquakeApp::mouseWheel( MouseEvent event )
{
	//mPov.adjustDist( event.getWheelIncrement() * -2.0f );
}


void EarthquakeApp::mouseMove( MouseEvent event )
{
    //	static bool firstMouseMove = true;
    //	if( ! firstMouseMove )
    //		mLastMouse = mCurrentMouse;
    //	else {
    //		mLastMouse = event.getPos();
    //		firstMouseMove = false;
    //	}
    //	mCurrentMouse = event.getPos();;
    //
    //	float xd = ( mCurrentMouse.x - mLastMouse.x ) * 0.005f;
    //
    ////	mPov.adjustAngle( -xd, mCurrentMouse.y - ( getWindowHeight() * 0.1f ) );
    //    std::cout<<"Mouse vel : "<<xd<<std::endl;
    
}

void EarthquakeApp::processInteractor()
{
   	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread
    /*
     * Update interactor
     */
    while(can_process_interactor)   interactor.process();
}

void EarthquakeApp::update()
{
	mPov.update();
	mPov.mCam.getBillboardVectors( &sBillboardRight, &sBillboardUp );
	
	//mLightDir = Vec3f( sin( mCounter ), 0.25f, cos( mCounter ) );
	mEarth.update();
	
	mCounter += 0.1f;
    

    
    hands = interactor.get_hands();
    
    if( hands.first.is_present() )
    {
        
        Vec2f hand_pos = Vec2f( hands.first.get_pos().x * getWindowWidth() / 320.0 ,
                               hands.first.get_pos().y * getWindowHeight() / 240.0 );
        
        Vec2f hand_vel = Vec2f( hands.first.get_vel().x , hands.first.get_vel().y );
        
        float z_vel = hands.first.get_vel().z ;
        
        int hand_gesture = hands.first.get_gesture();
        int static_gesture = interactor.get_gesture().id;
        
        std::cout << hand_vel <<" /// " << z_vel << " --- "<<hand_gesture<<std::endl;
        
        /*
         * If lateral movement, pan
         */
        /// \todo Block panning if L or Victory?
        if( hand_gesture == GEST_VICTORY && block_timer.getSeconds() > BLOCK_TIMEOUT )
        {
            static bool firstMouseMove = true;
            if( ! firstMouseMove )
                mLastMouse = mCurrentMouse;
            else {
                mLastMouse = hand_pos;//event.getPos();
                firstMouseMove = false;
            }
            mCurrentMouse = hand_pos;//event.getPos();;
            
            if( hand_vel.x == hand_vel.x ) /*not NaN*/
            {
                //mPov.adjustAngle( -hand_vel.x * 0.005, mCurrentMouse.y*0  + getWindowHeight() * 0.5 - ( getWindowHeight() * 0.1f ) );
             //   std::cout<< hand_vel <<std::endl;
                mPov.adjustAngles( -hand_vel.x * 0.005, hand_vel.y * 0.004 ) ;
            }
            
        }
        
        
        
//        /*
//         * If gesture "GEST_VICTORY", zoom in/out
//         */
//        if( z_vel == z_vel  &&
//            fabs( z_vel ) > MIN_Z_VEL &&
//            hand_gesture == GEST_VICTORY &&
//            block_timer.getSeconds() > BLOCK_TIMEOUT )
//        {
//            float sign = z_vel / fabs(z_vel) ;
//           // mPov.adjustDist( -sign * 250.0 );
//            mPov.incDist( -sign * 0.25 );
//            block_timer.start();
//        }
       
        if( static_gesture == 2 && block_timer.getSeconds() > BLOCK_TIMEOUT )
        {
            mShowQuakes = ! mShowQuakes;
            block_timer.start();
        }
        
        last_hand_gesture = hand_gesture ;
    }
    
}

void EarthquakeApp::draw()
{
	glClearColor( 1.0f, 0.0f, 0.0f, 1.0f );
	
	gl::enableAlphaBlending();
	gl::enableDepthRead( true );
	gl::enableDepthWrite( true );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glEnable( GL_TEXTURE_2D );
	glDisable( GL_TEXTURE_RECTANGLE_ARB );
	gl::GlslProg::unbind();
	
	glColor4f( 1, 1, 1, 1 );
	mStars.enableAndBind();
	gl::drawSphere( Vec3f( 0, 0, 0 ), 15000.0f, 64 );
	
	//gl::rotate( Quatf( Vec3f::zAxis(), -0.2f ) );
	//gl::rotate( Quatf( Vec3f::yAxis(), mCounter*0.1f ) );
	
	if( mShowEarth ){
		mEarthShader.bind();
		mEarthShader.uniform( "texDiffuse", 0 );
		mEarthShader.uniform( "texNormal", 1 );
		mEarthShader.uniform( "texMask", 2 );
		mEarthShader.uniform( "counter", mCounter );
		mEarthShader.uniform( "lightDir", mLightDir );
		mEarth.draw();
		mEarthShader.unbind();
	}
	
	
	glDisable( GL_TEXTURE_2D );
	
	
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    
	if( mShowQuakes ){
		mQuakeShader.bind();
		mQuakeShader.uniform( "lightDir", mLightDir );
		mEarth.drawQuakeVectors();
		mQuakeShader.unbind();
	}
	
	if( mShowText ){
		gl::enableDepthWrite( false );
		glEnable( GL_TEXTURE_2D );
		//mEarth.drawQuakeLabelsOnBillboard( sBillboardRight, sBillboardUp );
		mEarth.drawQuakeLabelsOnSphere( mPov.mEyeNormal, mPov.mDist );
		glDisable( GL_TEXTURE_2D );
	}
	
	if( mSaveFrames ){
		writeImage( getHomeDirectory() / "CinderScreengrabs" / ( "Highoutput_" + toString( mCurrentFrame ) + ".png" ), copyWindowSurface() );
		mCurrentFrame++;
	}
}

void EarthquakeApp::parseEarthquakes( const string &url )
{
	try {
		const JsonTree json( loadUrl( url ) );
		for( auto &feature : json["features"].getChildren() ) {
			auto &coords = feature["geometry"]["coordinates"];
			float mag = feature["properties"]["mag"].getValue<float>();
			string title = feature["properties"]["title"].getValue();
			mEarth.addQuake( coords[0].getValue<float>(), coords[1].getValue<float>(), mag, title );
		}
	}
	catch( ... ) {
		console() << "Failed to parse." << std::endl;
	}
	
	//mEarth.addQuake( 37.7f, -122.0f, 8.6f, "San Francisco" );
}

void EarthquakeApp::shutdown()
{
    can_process_interactor = false;
	mThread->join();
}


CINDER_APP_BASIC( EarthquakeApp, RendererGl )
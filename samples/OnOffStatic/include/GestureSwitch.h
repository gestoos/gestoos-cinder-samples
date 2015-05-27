#ifndef GestureSwitch_Label_h
#define GestureSwitch_Label_h

#include "cinder/Cinder.h"
#include "cinder/Rand.h"
#include "cinder/app/AppNative.h"
#include "cinder/Text.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"

#include "Cinderactor.h"
#include "MovingRect.h"

#include <list>

using namespace ci;
using namespace ci::app;


/// \brief Class implementing on/off switches
class GestureSwitch {

public:

	/// Empty constructor
	GestureSwitch();

	/// Constructor with a gesture ID
	GestureSwitch(int _id, cinder::DataSourceRef _rid, Rectf _pos);

	/// Default destructor
	virtual ~GestureSwitch();

	/// "Push" the switch
	void switch_update();

	/// Draw the switch
	void draw();

	/// Get the gesture ID
	int get_id();

private:

	int id;
	bool on;

	gl::Texture image;

	Rectf pos;
	Timer timer;
};

#endif
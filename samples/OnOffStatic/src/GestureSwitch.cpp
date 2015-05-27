#include "GestureSwitch.h"

GestureSwitch::GestureSwitch()
{
	on = false;
	pos = Rectf(0, 0, 200, 200);
	timer.start();
}

GestureSwitch::GestureSwitch(int _id, int _rid, Rectf _pos)
{
	id = _id;
	pos = _pos;
	on = false;
	image = gl::Texture(loadImage(loadResource(_rid, "PNG")));
	timer.start();
}

void GestureSwitch::switch_update()
{
	if (timer.getSeconds() < 1.0) return;
	on = !on;
	timer.start();
}

void GestureSwitch::draw()
{
	if (!on)
		gl::color(1.0, 1.0, 1.0, 0.2);
	else
		gl::color(1.0, 1.0, 1.0, 1.0);

	gl::draw(image, pos);
}

int GestureSwitch::get_id()
{
	return id;
}

GestureSwitch::~GestureSwitch()
{

}
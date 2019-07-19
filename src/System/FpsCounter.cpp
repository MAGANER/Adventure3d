#include "FpsCounter.h"



FpsCounter::FpsCounter()
{
	if (!font.loadFromFile("data/font/opensans.ttf")) 
	{
		fprintf(stderr, "Cannot open font!\n");
		exit(-1);
	}

	dt_counter = 0.0f;
	frame_counter = 0;
	frame_time_microsecs = 0;
	fps_refresh_time = 0.1f;

	//info_string_size = sizeof("FPS: *****.*, Frame time: ******");
	//frame_info_string[info_string_size] = new char;

	fps_text = new Text("", font, 50);
}
FpsCounter::~FpsCounter()
{
	//delete[] frame_info_string;
	delete fps_text;
}

void FpsCounter::count()
{
	// get delta time
	float dt = clock.restart().asSeconds();

	// Update FPS, smoothed over time
	if (dt_counter >= fps_refresh_time) {
		float fps = (float)frame_counter / dt_counter;
		frame_time_microsecs /= frame_counter;
		frame_info_string ="FPS:"+ to_string((int)fps) + "Frame time:";
		fps_text->setString(frame_info_string);
		dt_counter = 0.0f;
		frame_counter = 0;
		frame_time_microsecs = 0;
	}
	dt_counter += dt;
	++frame_counter;
}
float FpsCounter::get_delta_time()
{
	return dt_counter;
}
void FpsCounter::draw(RenderWindow* window)
{
	window->draw(*fps_text);
}
void FpsCounter::update_frame_time()
{
	frame_time_microsecs += clock.getElapsedTime().asMicroseconds();
}
#include "FpsCounter.h"



FpsCounter::FpsCounter()
{
	if (!font.loadFromFile("data/font/opensans.ttf")) 
	{
		cout << stderr << "can not open font!" << endl <<"programm will be closed in 5 seconds!";

		Time time_to_wait = seconds(5.0f);
		sleep(time_to_wait); // to make user able to read message
		exit(-1);
	}

	dt_counter = 0.0f;
	frame_counter = 0;
	frame_time_microsecs = 0;
	fps_refresh_time = 0.1f;

	fps_text = new Text("", font, 50);
}
FpsCounter::~FpsCounter()
{
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
		frame_info_string ="FPS:"+ to_string((int)fps);
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
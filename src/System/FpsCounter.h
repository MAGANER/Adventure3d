#pragma once
#include"stdafx.h"
class FpsCounter
{
private:
	Clock clock; // timer
	Text* fps_text;
	Font font;

	float dt_counter;// delta time for multiple frames, for calculating FPS smoothly
	int frame_counter;// counts frames for FPS calculation
	long long frame_time_microsecs;// time needed to draw frames
    float fps_refresh_time; // time between FPS text refresh. FPS is smoothed out over this time


	string frame_info_string;// string buffer for frame information
public:
	FpsCounter();
	~FpsCounter();

	void count();
	void draw(RenderWindow* window);
	void update_frame_time();
	float get_delta_time();
};


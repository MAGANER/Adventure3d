#pragma once
#include"System\FpsCounter.h"
#include"System\Map.h"
#include"Objects/Player.h"
#include"System\Renderer.h"
#include<iostream>
using namespace std;

class Game
{
private:
	Map level;
	Renderer renderer;
	FpsCounter fps_counter;
	Player* hero;
	RenderWindow* window;

	bool hasFocus;
public:
	Game();
	~Game();
	void run();
private:
	void check_map_for_errors();
	void handle_SFML_events();
};


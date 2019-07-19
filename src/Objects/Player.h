#pragma once
#include"stdafx.h"
#include"System\Map.h"
#include"System\RotateVec.h"
class Player
{
private:
	Vector2f position; // coordinates in worldMap
	Vector2f direction; // direction, relative to (0,0)
	Vector2f plane; /* 2d raycaster version of the camera plane,
                       must be perpendicular to rotation  */

	Vector2f collision_box;

	float size_f; // dimensions of player collision box, in tiles
	float move_speed; // player movement speed in tiles per second
	float rotate_speed; // player rotation speed in radians per second

	float camera_height;// height of player camera(1.0 is ceiling, 0.0 is floor)
public:
	Player(Vector2f position, Vector2f direction, Vector2f plane);
	virtual ~Player();

	//get keyboard input and handle movement/rotation
	void move(bool has_focus, float dt, Map level);
	void rotate(bool has_focus, float dt);

	Vector2f get_position();
	Vector2f get_direction();
	Vector2f get_plane();

	float get_camera_height();
private:
	bool can_move(Vector2f position,Map level);
};


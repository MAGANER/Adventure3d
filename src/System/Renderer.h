#pragma once
#include"stdafx.h"
#include"Objects/Player.h"
class Renderer
{
private:
	int screenWidth, screenHeight;

	int texture_size;      // size(width and height) of texture that will hold all wall textures
	int texture_wall_size; // size(width and height) of each wall type in the full texture

	Texture texture;    //contains all textures
	RenderStates* state;// render state that uses the texture
	VertexArray* lines; // lines used to draw walls and floors on the screen
private:
	// ray to emit
	Vector2f rayPos, rayDir;
	float cameraX;// x in camera space (between -1 and +1)
	void calculate_ray_parameters(int x, Player hero); // position and direction

private:
	Vector2f deltaDist;// calculate distance traversed between each grid line for x and y based on direction
	void calculate_deltaDist();

	Vector2i mapPos;// which box of the map we're in
	void calculate_map_pos();

	Vector2i step; // what direction to step in (+1 or -1 for each dimension)
	Vector2f sideDist; // distance from current position to next gridline, for x and y separately
	void calculate_step_with_sideDist();

	char hit_tile;   // tile type that got hit
	bool is_hit_side_horizontal; // did we hit a horizontal side? Otherwise it's vertical

	float perpWallDist;// wall distance, projected on camera direction
	int wallHeight; // height of wall to draw on the screen at each distance
	int ceilingPixel; // position of ceiling pixel on the screen
	int groundPixel; // position of ground pixel on the screen

	void cast_ray(int x, Player hero, Map level);

	//lowest and highest pixel, that fills in current line
	int drawStart;
	int drawEnd;
	void calculate_filling_pixels();

	Vector2f current_texture;
	void calculate_texture_coords(Map level);

	void calculate_hit_place();
	void add_line_to_vertex_buffer(int x);


	Color floor_color; // current color

	Color first_floor_color;  // variant 1
	Color second_floor_color; // variant 2

	Color get_floor_color(); // calculate current color
public:
	Renderer();
	~Renderer();

	void draw(Player hero,Map level, RenderWindow* window);

	Vector2i get_screen_size();
};


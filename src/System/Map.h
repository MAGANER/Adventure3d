#pragma once
#include"stdafx.h"
#include <unordered_map>

class Map
{
private:
	// list of wall texture types, in order as they appear in the full texture
	enum class WallTexture {
		Smiley,
		Red,
		Bush,
		Sky,
		Pink,
		Wallpaper,
		Dirt,
		Exit,
	};
	// valid wall types and their texture for the world map
	unordered_map<char, WallTexture> wallTypes;
private:
	int width, height;
	string world;
public:
	Map();
	~Map();

	char get_tile(int x, int y);
	bool check_for_errors();// returns: true on success, false on errors found

	int get_width();
	int get_height();
	unordered_map<char, WallTexture> get_wall_types();


	//TODO: LOADING
};


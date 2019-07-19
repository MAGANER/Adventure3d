#include "Map.h"



Map::Map()
{
	width = 24;
	height = 24;

	//top down view. FOR TESTING ONLY
	world = 
		"~~~~~~~~~~~~~~~~MMM@MMMM"
		"~..............=M......M"
		"~..............=M......M"
		"~..............=@......@"
		"~..............=M......M"
		"~....N......N..........M"
		"~..............=MMM@MM.M"
		"~..............======M.M"
		"~..............=MMMMMM.M"
		"~..............=M......M"
		"~...N....N.....=M..N..M#"
		"~.....................M#"
		"~..............=M..N..M#"
		"~..............=M.....M#"
		"~...........N..=MMMMM.M#"
		"~..............======.=#"
		"#.!!!!!!.!!!!!!........#"
		"#.!....!.!..........=..#"
		"#.!.N..!.!..==..=...=..#"
		"#...........==..==..=..#"
		"#.!!!!!!.!..==.........#"
		"#.######.#..==....=....#"
		"N......................^"
		"########################";


	wallTypes =
	{
		{ '#', WallTexture::Pink },
		{ '=', WallTexture::Dirt },
		{ 'M', WallTexture::Wallpaper },
		{ 'N', WallTexture::Bush },
		{ '~', WallTexture::Sky },
		{ '!', WallTexture::Red },
		{ '@', WallTexture::Smiley },
		{ '^', WallTexture::Exit }
	};


}
Map::~Map()
{
}

char Map::get_tile(int x, int y)
{
	//not memory safe!
	return world[y * width + x];
}
bool Map::check_for_errors()
{
	// check size
	if (world.size() != width * height) {
		cout << stderr << "map size:" << world.size() << "is not " << height*width << endl;
		return false;
	}

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			char tile = get_tile(x, y);
			// check if tile type is valid
			if (tile != '.' && wallTypes.find(tile) == wallTypes.end()) {
				fprintf(stderr, "map tile at [%3d,%3d] has an unknown tile type(%c)\n", x, y, tile);
				return false;
			}
			// check if edges are walls
			if ((y == 0 || x == 0 || y == height - 1 || x == width - 1) &&
				tile == '.') {
				fprintf(stderr, "map edge at [%3d,%3d] is a floor (should be wall)\n", x, y);
				return false;
			}
		}
	}
	return true;
}

int Map::get_width()
{
	return width;
}
int Map::get_height()
{
	return height;
}
unordered_map<char, Map::WallTexture> Map::get_wall_types()
{
	return wallTypes;
}

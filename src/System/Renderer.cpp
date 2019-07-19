#include "Renderer.h"
Renderer::Renderer()
{
	screenWidth  = 1280;
	screenHeight = 720;

	texture_size = 512;
	texture_wall_size = 128;

	if (!texture.loadFromFile("data/texture/walls.png")) 
	{
		cout << stderr << "can not open texture!";

		//wait
		Time sleep_t = seconds(5.0f);
		sleep(sleep_t);

		exit(-1);
	}
	state = new RenderStates(&texture);
	lines = new VertexArray(sf::Lines, 18 * screenWidth);


	first_floor_color  = sf::Color::Green;
	second_floor_color = sf::Color::Cyan;

	perpWallDist = 0.0f;
	ceilingPixel = 0;
	groundPixel = screenHeight;

	hit_tile = '.';
}
Renderer::~Renderer()
{
	delete state;
	delete lines;
}
void Renderer::draw(Player hero, Map level, RenderWindow* window)
{	
	lines->resize(0);
	for (int x = 0; x < screenWidth; ++x)
	{
		calculate_ray_parameters(x, hero);
		calculate_deltaDist();
		calculate_map_pos();
		calculate_step_with_sideDist();
	    
		//set to start values before raycasting
		hit_tile = '.';
		perpWallDist = 0.0f;
		ceilingPixel = 0;
		groundPixel = screenHeight;

		//calculate floor color
		floor_color = get_floor_color();
		cast_ray(x, hero, level);

		calculate_filling_pixels();
		calculate_texture_coords(level);
		calculate_hit_place();
		add_line_to_vertex_buffer(x);

	}
	window->draw(*lines, *state);
	
}
void Renderer::calculate_ray_parameters(int x, Player hero)
{
	cameraX = 2 * x / (float)screenWidth - 1.0f;

	rayPos = hero.get_position();
	rayDir = hero.get_direction() + hero.get_plane() * cameraX;
}
void Renderer::calculate_map_pos()
{
	mapPos = Vector2i(rayPos);
}
void Renderer::calculate_deltaDist()
{
	deltaDist.x = sqrt(1.0f + (rayDir.y * rayDir.y) / (rayDir.x * rayDir.x));
	deltaDist.y = sqrt(1.0f + (rayDir.x * rayDir.x) / (rayDir.y * rayDir.y));
}
void Renderer::calculate_step_with_sideDist()
{
	if (rayDir.x < 0.0f) {
		step.x = -1;
		sideDist.x = (rayPos.x - mapPos.x) * deltaDist.x;
	}
	else {
		step.x = 1;
		sideDist.x = (mapPos.x + 1.0f - rayPos.x) * deltaDist.x;
	}
	if (rayDir.y < 0.0f) {
		step.y = -1;
		sideDist.y = (rayPos.y - mapPos.y) * deltaDist.y;
	}
	else {
		step.y = 1;
		sideDist.y = (mapPos.y + 1.0f - rayPos.y) * deltaDist.y;
	}
}
void Renderer::cast_ray(int x, Player hero, Map level)
{
	// cast the ray until we hit a wall, meanwhile draw floors
	while (hit_tile == '.') {
		if (sideDist.x < sideDist.y) {
			sideDist.x += deltaDist.x;
			mapPos.x += step.x;
			is_hit_side_horizontal = true;
			perpWallDist = (mapPos.x - rayPos.x + (1 - step.x) / 2) / rayDir.x;
		}
		else {
			sideDist.y += deltaDist.y;
			mapPos.y += step.y;
			is_hit_side_horizontal = false;
			perpWallDist = (mapPos.y - rayPos.y + (1 - step.y) / 2) / rayDir.y;
		}

		wallHeight = screenHeight / perpWallDist;

		// add floor

		lines->append(sf::Vertex(sf::Vector2f((float)x, (float)groundPixel), floor_color, sf::Vector2f(385.0f, 129.0f)));
		groundPixel = int(wallHeight * hero.get_camera_height() + screenHeight * 0.5f);
		lines->append(sf::Vertex(sf::Vector2f((float)x, (float)groundPixel), floor_color, sf::Vector2f(385.0f, 129.0f)));

		// add ceiling
		sf::Color color_c = floor_color;
		color_c.r /= 2;
		color_c.g /= 2;
		color_c.b /= 2;

		lines->append(sf::Vertex(sf::Vector2f((float)x, (float)ceilingPixel), color_c, sf::Vector2f(385.0f, 129.0f)));
		ceilingPixel = int(-wallHeight * (1.0f - hero.get_camera_height()) + screenHeight * 0.5f);
		lines->append(sf::Vertex(sf::Vector2f((float)x, (float)ceilingPixel), color_c, sf::Vector2f(385.0f, 129.0f)));

		// change color and find tile type
		floor_color = (floor_color == first_floor_color) ? second_floor_color : first_floor_color;

		hit_tile = level.get_tile(mapPos.x, mapPos.y);
	}
}
void Renderer::calculate_filling_pixels()
{
	drawStart = ceilingPixel;
	drawEnd = groundPixel;
}
Color Renderer::get_floor_color()
{
	//choose current floor color
	Color color;
	bool first_case  = (mapPos.x % 2 == 0 && mapPos.y % 2 == 0);
	bool second_case = (mapPos.x % 2 == 1 && mapPos.y % 2 == 1);
	color = (first_case || second_case) ? first_floor_color : second_floor_color;
	return color;
}
void Renderer::calculate_texture_coords(Map level)
{
	int wallTextureNum = (int)level.get_wall_types().find(hit_tile)->second;

	current_texture = Vector2f(
		wallTextureNum * texture_wall_size % texture_size,
		wallTextureNum * texture_wall_size / texture_size * texture_wall_size
		);
}
void Renderer::calculate_hit_place()
{
	// calculate where the wall was hit
	float wall_x;
	if (is_hit_side_horizontal) {
		wall_x = rayPos.y + perpWallDist * rayDir.y;
	}
	else {
		wall_x = rayPos.x + perpWallDist * rayDir.x;
	}
	wall_x -= floor(wall_x);

	// get x coordinate on the wall texture
	int tex_x = int(wall_x * float(texture_wall_size));

	// flip texture if we see it on the other side of us, this prevents a mirrored effect for the texture
	if ((is_hit_side_horizontal && rayDir.x <= 0) || (!is_hit_side_horizontal && rayDir.y >= 0)) {
		tex_x = texture_wall_size - tex_x - 1;
	}

	current_texture.x += tex_x;
}
void Renderer::add_line_to_vertex_buffer(int x)
{
	// illusion of shadows by making horizontal walls darker
	Color color = sf::Color::White;
	if (is_hit_side_horizontal) {
		color.r /= 2;
		color.g /= 2;
		color.b /= 2;
	}

	// add line to vertex buffer
	lines->append(sf::Vertex(
		sf::Vector2f((float)x, (float)drawStart),
		color,
		sf::Vector2f((float)current_texture.x, (float)current_texture.y + 1)
		));
	lines->append(sf::Vertex(
		sf::Vector2f((float)x, (float)drawEnd),
		color,
		sf::Vector2f((float)current_texture.x, (float)(current_texture.y + texture_wall_size - 1))
		));
}
Vector2i Renderer::get_screen_size()
{
	Vector2i size;
	size.x = screenWidth;
	size.y = screenHeight;

	return size;
}
// Raycasting 3D adventure game, based on http://lodev.org/cgtutor/raycasting.html
// Inspired by seeing https://github.com/TheMozg/awk-raycaster
//
// Copyright Thomas van der Berg, 2016, some parts taken from aforementioned tutorial copyrighted by its author
// Licensed under GNU GPLv3 (see LICENSE), tutorial parts liberally licensed(see LICENSE.tutorial)

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <unordered_map>
#include"System\FpsCounter.h"
#include"System\Map.h"
#include"Player.h"
#include<iostream>
using namespace std;



Map level;

const int screenWidth = 1280;
const int screenHeight = 720;

const float cameraHeight = 0.66f; // height of player camera(1.0 is ceiling, 0.0 is floor)

const int texture_size = 512; // size(width and height) of texture that will hold all wall textures
const int texture_wall_size = 128; // size(width and height) of each wall type in the full texture

// check if a rectangular thing with given size can move to given position without colliding with walls or
// being outside of the map
// position is considered the middle of the rectangle
bool canMove(sf::Vector2f position, sf::Vector2f size) {
    // create the corners of the rectangle
    sf::Vector2i upper_left(position - size / 2.0f);
    sf::Vector2i lower_right(position + size / 2.0f);
	if (upper_left.x < 0 || upper_left.y < 0 || lower_right.x >= level.get_width() || lower_right.y >= level.get_height()) {
        return false; // out of map bounds
    }
    // loop through each map tile within the rectangle. The rectangle could be multiple tiles in size!
    for (int y = upper_left.y; y <= lower_right.y; ++y) {
        for (int x = upper_left.x; x <= lower_right.x; ++x) {
            if (level.get_tile(x, y) != '.') {
                return false;
            }
        }
    }
    return true;
}



int main() {

     //if the map is not correct, we can have segmentation faults. So check it.
    if (!level.check_for_errors()) {
		cout << stderr << "map is invalid!";
		Time sleep_t = seconds(5.0f);
		sleep(sleep_t);
        return EXIT_FAILURE;
    }

    sf::Texture texture;
    if (!texture.loadFromFile("data/texture/walls.png")) {
        fprintf(stderr, "Cannot open texture!\n");
        return EXIT_FAILURE;
    }

    // render state that uses the texture
    sf::RenderStates state(&texture);


	//create hero with his start parameters
	Player hero(Vector2f(2.5f, 2.0f),    //position
		        Vector2f(0.0f, 1.0f),    //direction
		        Vector2f(-0.66f, 0.0f)); //plane

    // create window
    sf::RenderWindow window(sf::VideoMode(screenWidth + 1, screenHeight), "Adventure 3D");
    window.setSize(sf::Vector2u(screenWidth, screenHeight)); // why add +1 and then set the size correctly?
                                                             // Fixes some problem with the viewport. If you
                                                             // don't do it, you'll see lots of gaps. Maybe
                                                             // there's a better fix.

    window.setFramerateLimit(60);
    bool hasFocus = true;

    // lines used to draw walls and floors on the screen
    sf::VertexArray lines(sf::Lines, 18 * screenWidth);


	FpsCounter fps_counter;

    while (window.isOpen()) {
		fps_counter.count();

        // handle SFML events
        sf::Event event;
        while (window.pollEvent(event)) {
            switch(event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::LostFocus:
                hasFocus = false;
                break;
            case sf::Event::GainedFocus:
                hasFocus = true;
                break;
            default:
                break;
            }
        }
      


		float dt = fps_counter.get_delta_time();
		hero.move(hasFocus, dt, level); //also check can hero move
		hero.rotate(hasFocus, dt);


        lines.resize(0);

        // loop through vertical screen lines, draw a line of wall for each
        for (int x = 0; x < screenWidth; ++x) {

            // ray to emit
            float cameraX = 2 * x / (float)screenWidth - 1.0f; // x in camera space (between -1 and +1)

            sf::Vector2f rayPos = hero.get_position();
            sf::Vector2f rayDir = hero.get_direction() + hero.get_plane() * cameraX;

            // NOTE: with floats, division by zero gives you the "infinity" value. This code depends on this.

            // calculate distance traversed between each grid line for x and y based on direction
            sf::Vector2f deltaDist(
                    sqrt(1.0f + (rayDir.y * rayDir.y) / (rayDir.x * rayDir.x)),
                    sqrt(1.0f + (rayDir.x * rayDir.x) / (rayDir.y * rayDir.y))
            );

            sf::Vector2i mapPos(rayPos); // which box of the map we're in

            sf::Vector2i step; // what direction to step in (+1 or -1 for each dimension)
            sf::Vector2f sideDist; // distance from current position to next gridline, for x and y separately

            // calculate step and initial sideDist
            if (rayDir.x < 0.0f) {
                step.x = -1;
                sideDist.x = (rayPos.x - mapPos.x) * deltaDist.x;
            } else {
                step.x = 1;
                sideDist.x = (mapPos.x + 1.0f - rayPos.x) * deltaDist.x;
            }
            if (rayDir.y < 0.0f) {
                step.y = -1;
                sideDist.y = (rayPos.y - mapPos.y) * deltaDist.y;
            } else {
                step.y = 1;
                sideDist.y = (mapPos.y + 1.0f - rayPos.y) * deltaDist.y;
            }

            char tile = '.'; // tile type that got hit
            bool horizontal; // did we hit a horizontal side? Otherwise it's vertical

            float perpWallDist = 0.0f; // wall distance, projected on camera direction
            int wallHeight; // height of wall to draw on the screen at each distance
            int ceilingPixel = 0; // position of ceiling pixel on the screen
            int groundPixel = screenHeight; // position of ground pixel on the screen

            // colors for floor tiles
            sf::Color color1 = sf::Color::Green;
            sf::Color color2 = sf::Color::Cyan;

            // current floor color
            sf::Color color = ((mapPos.x % 2 == 0 && mapPos.y % 2 == 0) ||
                               (mapPos.x % 2 == 1 && mapPos.y % 2 == 1)) ? color1 : color2;

            // cast the ray until we hit a wall, meanwhile draw floors
            while (tile == '.') {
                if (sideDist.x < sideDist.y) {
                    sideDist.x += deltaDist.x;
                    mapPos.x += step.x;
                    horizontal = true;
                    perpWallDist = (mapPos.x - rayPos.x + (1 - step.x) / 2) / rayDir.x;
                } else {
                    sideDist.y += deltaDist.y;
                    mapPos.y += step.y;
                    horizontal = false;
                    perpWallDist = (mapPos.y - rayPos.y + (1 - step.y) / 2) / rayDir.y;
                }

                wallHeight = screenHeight / perpWallDist;

                // add floor

                lines.append(sf::Vertex(sf::Vector2f((float)x, (float)groundPixel), color, sf::Vector2f(385.0f, 129.0f)));
                groundPixel = int(wallHeight * cameraHeight + screenHeight * 0.5f);
                lines.append(sf::Vertex(sf::Vector2f((float)x, (float)groundPixel), color, sf::Vector2f(385.0f, 129.0f)));

                // add ceiling

                sf::Color color_c = color;
                color_c.r /= 2;
                color_c.g /= 2;
                color_c.b /= 2;

                lines.append(sf::Vertex(sf::Vector2f((float)x, (float)ceilingPixel), color_c, sf::Vector2f(385.0f, 129.0f)));
                ceilingPixel = int(-wallHeight * (1.0f - cameraHeight) + screenHeight * 0.5f);
                lines.append(sf::Vertex(sf::Vector2f((float)x, (float)ceilingPixel), color_c, sf::Vector2f(385.0f, 129.0f)));

                // change color and find tile type

                color = (color == color1) ? color2 : color1;

                tile = level.get_tile(mapPos.x, mapPos.y);
            }

            // calculate lowest and highest pixel to fill in current line
            int drawStart = ceilingPixel;
            int drawEnd = groundPixel;

            // get position of the wall texture in the full texture
            int wallTextureNum = (int)level.get_wall_types().find(tile)->second;
            sf::Vector2i texture_coords(
                    wallTextureNum * texture_wall_size % texture_size,
                    wallTextureNum * texture_wall_size / texture_size * texture_wall_size
            );

            // calculate where the wall was hit
            float wall_x;
            if (horizontal) {
                wall_x = rayPos.y + perpWallDist * rayDir.y;
            } else {
                wall_x = rayPos.x + perpWallDist * rayDir.x;
            }
            wall_x -= floor(wall_x);

            // get x coordinate on the wall texture
            int tex_x = int(wall_x * float(texture_wall_size));

            // flip texture if we see it on the other side of us, this prevents a mirrored effect for the texture
            if ((horizontal && rayDir.x <= 0) || (!horizontal && rayDir.y >= 0)) {
                tex_x = texture_wall_size - tex_x - 1;
            }

            texture_coords.x += tex_x;

            // illusion of shadows by making horizontal walls darker
            color = sf::Color::White;
            if (horizontal) {
                color.r /= 2;
                color.g /= 2;
                color.b /= 2;
            }

            // add line to vertex buffer
            lines.append(sf::Vertex(
                        sf::Vector2f((float)x, (float)drawStart),
                        color,
                        sf::Vector2f((float)texture_coords.x, (float)texture_coords.y + 1)
            ));
            lines.append(sf::Vertex(
                        sf::Vector2f((float)x, (float)drawEnd),
                        color,
                        sf::Vector2f((float)texture_coords.x, (float)(texture_coords.y + texture_wall_size - 1))
            ));
        }

        window.clear();
        window.draw(lines, state);
		fps_counter.draw(&window);
		fps_counter.update_frame_time();
        window.display();
    }

    return EXIT_SUCCESS;
}

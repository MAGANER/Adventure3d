#include "Player.h"



Player::Player(Vector2f position, Vector2f direction, Vector2f plane)
{
	this->position = position;
	this->direction = direction;
	this->plane = plane;


	size_f = 0.375f;
	move_speed = 2.0f;
	rotate_speed = 1.0f;
	camera_height = 0.66f;

	collision_box.x = size_f;
	collision_box.y = size_f;
}
Player::~Player()
{
}
bool Player::can_move(Vector2f position, Map level)
{
	// create the corners of the rectangle
	sf::Vector2i upper_left(position - collision_box / 2.0f);
	sf::Vector2i lower_right(position + collision_box / 2.0f);
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
void Player::move(bool has_focus, float dt, Map level)
{
	if (has_focus)
	{
		using kb = sf::Keyboard;

		// moving forward or backwards (1.0 or -1.0)
		float moveForward = 0.0f;

		// get input
		if (kb::isKeyPressed(kb::Up))
		{
			moveForward = 1.0f;
		}
		else if (kb::isKeyPressed(kb::Down))
		{
			moveForward = -1.0f;
		}


		// handle movement
		if (moveForward != 0.0f)
		{
			sf::Vector2f moveVec = direction * move_speed * moveForward * dt;

			if (can_move(sf::Vector2f(position.x + moveVec.x, position.y), level))
			{
				position.x += moveVec.x;
			}
			if (can_move(sf::Vector2f(position.x, position.y + moveVec.y), level))
			{
				position.y += moveVec.y;
			}
		}
	}
}
void Player::rotate(bool has_focus, float dt)
{
	if (has_focus)
	{
		using kb = sf::Keyboard;

		// rotating rightwards or leftwards(1.0 or -1.0)
		float rotateDirection = 0.0f;

		// get input
		if (kb::isKeyPressed(kb::Left)) {
			rotateDirection = -1.0f;
		}
		else if (kb::isKeyPressed(kb::Right)) {
			rotateDirection = 1.0f;
		}

		// handle rotation
		if (rotateDirection != 0.0f) {
			float rotation = rotate_speed * rotateDirection * dt;
			direction = rotateVec(direction, rotation);
			plane = rotateVec(plane, rotation);
		}
	}
}
Vector2f Player::get_direction()
{
	return direction;
}
Vector2f Player::get_position()
{
	return position;
}
Vector2f Player::get_plane()
{
	return plane;
}

float Player::get_camera_height()
{
	return camera_height;
}
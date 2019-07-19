#pragma once
#include"SFML\System\Vector2.hpp"
#include <math.h>
namespace
{
	// rotate a given vector with given float value in radians and return the result
	// see: https://en.wikipedia.org/wiki/Rotation_matrix#In_two_dimensions
	sf::Vector2f rotateVec(sf::Vector2f vec, float value) {
		return sf::Vector2f(
			vec.x * std::cos(value) - vec.y * std::sin(value),
			vec.x * std::sin(value) + vec.y * std::cos(value)
			);
	}

};
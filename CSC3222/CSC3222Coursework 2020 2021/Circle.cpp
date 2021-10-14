#include "Circle.h"
#include "../../Common/Maths.h"

/*
	Author: Tony Kingston
	File: Circle.cpp
	Last Updated: 22/5/2021

*/

using namespace NCL;
using namespace CSC3222;

/// <summary>
/// Constructor for the Circle class.
/// </summary>
/// <param name="point">Vector2 representing centre of the circle</param>
/// <param name="radius">float: radius of the Circle</param>
Circle::Circle(Vector2 point, float radius) : CollisionVolume(point), radius(radius) {
	// Set mix and max points based on centre and radius
	min = Vector2(point.x - radius, point.y - radius);
	max = Vector2(point.x + radius, point.y + radius);
}

/// <summary>
/// Destructor for the Circle class
/// </summary>
Circle::~Circle() {

}

/// <summary>
/// Calls the correct collision method depending on the CollisionVolume's type
/// </summary>
/// <param name="c">CollisionVolume to check collision with</param>
/// <returns>True if they collide, else false</returns>
bool Circle::Collides(CollisionVolume& c) {
	return c.Collides(*this);
}

/// <summary>
/// Returns the radius of the circle
/// </summary>
/// <returns>float: radius</returns>
float Circle::getRadius() const {
	return radius;
}

/// <summary>
/// Determines if the circle collides with an AABB
/// </summary>
/// <param name="box">AABB to check collision with</param>
/// <returns>True if they collide, else false</returns>
bool Circle::Collides(AABB& box) {

	if (box.Collides(*this)) {
		return true;
	}
	return false;
}

/// <summary>
/// Determines if the circle collides with another circle
/// </summary>
/// <param name="c">Circle to check collision with</param>
/// <returns>True if they collide, else false</returns>
bool Circle::Collides(Circle& c)  {
	float dx = centre.x - c.centre.x;
	float dy = centre.y - c.centre.y;
	float distance = sqrt(dx * dx + dy * dy);
	float sumRadi = radius + c.radius;

	if (distance < sumRadi) {
		return true;
	}
	return false;
}
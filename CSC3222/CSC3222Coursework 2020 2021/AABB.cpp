#include "AABB.h"
#include "../../Common/Maths.h"

/*
    Author: Tony Kingston
	File: AABB.cpp
	Last Updated: 22/5/2021

*/

using namespace NCL;
using namespace CSC3222;

AABB::AABB(Vector2 point, float halfSize) : CollisionVolume(point), halfWidth(halfSize), halfHeight(halfSize) {
	min = Vector2(point.x - halfSize, point.y - halfSize);
	max = Vector2(point.x + halfSize, point.y + halfSize);
}

AABB::AABB(Vector2 point, float halfWidth, float halfHeight) : CollisionVolume(point), halfWidth(halfWidth), halfHeight(halfHeight) {
	min = Vector2(point.x - halfWidth, point.y - halfHeight);
	max = Vector2(point.x + halfWidth, point.y + halfHeight);
}

AABB::~AABB() {

}

bool AABB::Collides(CollisionVolume& c) {
	return c.Collides(*this);
}

bool AABB::Collides(AABB& box) {
	if (centre.x + halfWidth >= box.centre.x &&
		centre.x <= box.centre.x + box.halfWidth &&
		centre.y + halfHeight >= box.centre.y &&
		centre.y <= box.centre.y + box.halfHeight) {
		return true;
	}

}

bool AABB::Collides(Circle& c) {
	float circleDistanceX = abs(c.getCentre().x - centre.x);
	float circleDistanceY = abs(c.getCentre().y - centre.y);

	if (circleDistanceX > halfWidth + c.getRadius() ||
		circleDistanceY > halfHeight + c.getRadius()) {
		return false;
	}

	if (circleDistanceX <= halfWidth || circleDistanceY <= halfHeight) {
		return true;
	}

	float cornerDistance = pow(circleDistanceX - halfWidth, 2) +
		pow(circleDistanceY - halfHeight, 2);

	return (cornerDistance <= pow(c.getRadius(), 2));
}
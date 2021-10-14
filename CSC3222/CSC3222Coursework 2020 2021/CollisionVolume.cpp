#include "CollisionVolume.h"
#include "../../Common/Vector2.h"

/*
	Modified by Tony Kingston
	File: CollisionVolume.cpp
	Last Updated: 22/5/2021

	Description:
	    Implementation of the CollisionVolume class. The class represents a bounding volume with some centre, min and max coordinates.

*/

using namespace NCL;
using namespace CSC3222;

/// <summary>
/// Constructor for the CollisionVolume class
/// </summary>
/// <param name="point">Centre of the collision volume</param>
CollisionVolume::CollisionVolume(Vector2 point): centre(point), collisionObj(NULL)
{

}

/// <summary>
/// Destructor for the CollisionVolume class
/// </summary>
CollisionVolume::~CollisionVolume()
{
}

/// <summary>
/// Gets centre of the collision volume
/// </summary>
/// <returns>Centre</returns>
Vector2 CollisionVolume::getCentre() const {
	return centre;
}

/// <summary>
/// Sets centre of the collision volume to a specified Vector2 position
/// </summary>
/// <param name="point">New centre</param>
void CollisionVolume::setCentre(Vector2 point) {
	centre = point;
}

/// <summary>
/// Update the centre, min and max points of the collision volume
/// </summary>
void CollisionVolume::updatePosition() {
	Vector2 newPos = collisionObj->GetPosition() + collisionObj->getOffset();
	Vector2 transform = newPos - centre;
	centre = newPos;
	min = min + transform;
	max = max + transform;

}

/// <summary>
/// Set the object that the volume is associated with
/// </summary>
/// <param name="object">Pointer to SimObject</param>
void CollisionVolume::setObject(SimObject* object) {
	collisionObj = object;
}

/// <summary>
/// Get the object that the collision volume is associated with
/// </summary>
/// <returns>Pointer to the SimObject that the volume is associated with</returns>
SimObject* CollisionVolume::getObject() const{
	return collisionObj;
}

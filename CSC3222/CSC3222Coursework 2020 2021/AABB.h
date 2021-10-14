#pragma once
#include "CollisionVolume.h"
#include "Circle.h"

/*
	Author: Tony Kingston
	File: AABB.h
	Last Updated: 22/5/2021

	Description:
		Header file of the AABB class. The class represents an axis-aligned bounding box with some halfWidth and halfHeight
		Methods are provided to check AABB/AABB collision and AABB/Circle collision

*/

namespace NCL {
	namespace CSC3222 {
		class AABB : public CollisionVolume {
		public:

			AABB(Vector2 point, float halfSize);
			AABB(Vector2 point, float halfWidth, float halfHeight);
			~AABB();

			// Double dispatch
			bool Collides(CollisionVolume& c);

			float halfWidth;
			float halfHeight;

			bool Collides(AABB& box);

			bool Collides(Circle& c);

		protected:
			
		};
	}
}
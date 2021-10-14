#pragma once
#include "CollisionVolume.h"
#include "AABB.h"

/*
	Author: Tony Kingston
	File: Circle.h
	Last Updated: 22/5/2021

	Description:
		Header file of the Circle class. The class represents a Circle with some radius
		Methods are provided to check Circle/Circle and Circle/AABB collision

*/

namespace NCL {
	namespace CSC3222 {
		class Circle : public CollisionVolume {
		public:

			Circle(Vector2 point, float radius);
			~Circle();
			// Double dispatch
			bool Collides(CollisionVolume& c);

			float getRadius() const;

			bool Collides(AABB& box);

			bool Collides(Circle& c);
		protected:

		protected:
			float radius;
		};
	}
}
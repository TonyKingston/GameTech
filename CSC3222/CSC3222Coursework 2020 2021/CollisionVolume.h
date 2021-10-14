#pragma once
#include "../../Common/Vector2.h"
#include "SimObject.h"

/*
	Modified by Tony Kingston
	File: CollisionVolume.h
	Last Updated: 22/5/2021

	Description:
		Header file of the CollisionVolume class. The class represents a bounding volume with some centre, min and max coordinates.
		The class defines pure virtual methods for collision which must be implemented by child classes. Double dispatch is used to call 
		the correct collision method for AABB and circle collision volumes.

*/

namespace NCL {
	using namespace Maths;
	namespace CSC3222 {
		class AABB;
		class Circle;

		class CollisionVolume {
			
		public:
			CollisionVolume(Vector2 point);
			~CollisionVolume();

			Vector2 getCentre() const;
			void setCentre(Vector2 point);

			// Double dispatch
			virtual bool Collides(CollisionVolume& x) = 0;
			void updatePosition();
			void setObject(SimObject* object);
			SimObject* getObject() const;

			// Storing min and max positions alongside centre. While it isn't necessary (since the mix and max points can always be calculated 
			// from the extents of the volume) it will reduce the number of calculations at the expensive of memory.
			Vector2 min;
			Vector2 max;
			
		protected:
			virtual bool Collides(AABB& a) = 0;
			virtual bool Collides(Circle& c) = 0;
			Vector2 centre;
			SimObject* collisionObj;

			friend class AABB;
			friend class Circle;

		};
	}
}


#pragma once
#include <vector>
#include "../../Common/Vector2.h"
#include "SimObject.h"

/*
	Modified by Tony Kingston
	File: GameSimsPhysics.h
	Last Updated: 22/5/2021

*/

namespace NCL {
	namespace CSC3222 {
		class RigidBody;
		class CollisionVolume;
		class AABB;
		class Circle;

		struct CollisionPair {
			Vector2    normal;
			float      penetration;
		};


		
		class GameSimsPhysics	{
		public:
			GameSimsPhysics();
			~GameSimsPhysics();

			void Update(float dt);

			void AddRigidBody(RigidBody* b);
			void RemoveRigidBody(RigidBody* b);

			void AddCollider(CollisionVolume* c);
			void RemoveCollider(CollisionVolume* c);
			bool Collides(CollisionVolume* a, CollisionVolume* b);

		protected:
			void Integration(float dt);
			void CollisionDetection(float dt);
			void ResolveCollision(CollisionVolume* a, CollisionVolume* b);
			void ImpulseResolution(CollisionVolume* a, CollisionVolume* b);
			void ProjectionResolution(CollisionVolume* a, CollisionVolume* b);
			bool Collides(AABB* a, AABB* b);
			bool Collides(Circle* a, Circle* b);
			bool Collides(AABB* a, Circle* b);
			bool Collides(Circle* a, AABB* b);
			static int compare(CollisionVolume* a, CollisionVolume* b);
			std::vector<RigidBody*>			allBodies;
			std::vector<RigidBody*>			allDynamicBodies;
			std::vector<CollisionVolume*>	allColliders;
			std::vector<CollisionVolume*> allDynamicColliders;

			CollisionPair cPair;
			
			// Used in alternate collison detection method
			std::vector<CollisionVolume*> allStaticColliders;
			static inline float sortAxis = 0;
		};
	}
}


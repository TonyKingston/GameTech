#pragma once
#include "../../Common/Vector2.h"

/*
	Modified by Tony Kingston
	File: RigidBody.h
	Last Updated: 22/5/2021

*/

namespace NCL {
	using namespace Maths;
	namespace CSC3222 {
		class RigidBody	{
			friend class GameSimsPhysics;
		public:
			RigidBody();
			~RigidBody();

			Vector2 GetPosition() const {
				return position;
			}

			void SetPosition(const Vector2& newPosition) {
				position = newPosition;
			}

			Vector2 GetVelocity() const {
				return velocity;
			}

			bool isStatic() const {
				return mIsStatic;
			}

			void SetVelocity(const Vector2& newVelocity) {
				velocity = newVelocity;
			}

			void AddForce(const Vector2& newForce) {
				force += newForce;
			}

			void SetInverseMass(float iMass) {
				inverseMass = iMass;
			}

			float GetInverseMass() {
				return inverseMass;
			}

			void AddImpulse(Vector2 newVelocity) {
				velocity = newVelocity;
			}

			float GetElasticity() {
				return elasticity;
			}

			Vector2 gravity = Vector2(0, -64.0f);

		protected:
			Vector2 position;
			Vector2 velocity;
			Vector2 force;
			float maxVelocity;

			float inverseMass;
			float elasticity;
			float damping;
			bool mIsStatic = false;

		};
	}
}

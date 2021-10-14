#pragma once
#include "SimObject.h"

/*
	Modified by Tony Kingston
	File: Spell.h
	Last Updated: 22/5/2021

*/

namespace NCL {
	namespace Maths {
		class Vector2;
	}
	namespace CSC3222 {
		class Spell : public SimObject {
		public:
			Spell(Vector2 direction);
			~Spell();

			void DrawObject(GameSimsRenderer &r) override;
			bool UpdateObject(float dt) override;
			void addBounce();
			
		protected:
			bool OnCollision(SimObject* x);
			float timer;
			int bounceCount;
		};
	}
}


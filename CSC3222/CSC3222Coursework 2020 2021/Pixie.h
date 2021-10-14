#pragma once
#include "SimObject.h"
#include <list>

/*
	Modified by Tony Kingston
	File: Pixie.h
	Last Updated: 22/5/2021

*/

namespace NCL::CSC3222 {
	class Pixie : public SimObject	{
	public:
		Pixie();
		~Pixie();

		bool UpdateObject(float dt) override;
		void attachToPlayer();
	protected:
		bool OnCollision(SimObject* obj);
	    void Flocking(float dt);
		Vector2 Alignment();
		Vector2 Separation();
		Vector2 Cohesion();
		Vector2 Avoidance();

		bool attached;
	};
}
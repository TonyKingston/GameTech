#pragma once
#include "SimObject.h"

/*
	Modified by Tony Kingston
	File: PixieDust.h
	Last Updated: 22/5/2021

*/

namespace NCL::CSC3222 {
	class PixieDust :	public SimObject	{
	public:
		PixieDust();
		~PixieDust();

		bool UpdateObject(float dt) override;
		

	protected:
		bool OnCollision(SimObject* obj);
	};
}

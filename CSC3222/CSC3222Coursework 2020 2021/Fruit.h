#pragma once
#include "SimObject.h"

/*
	Modified by Tony Kingston
	File: Fruit.h
	Last Updated: 22/5/2021

*/

namespace NCL::CSC3222 {
	class Fruit : public SimObject	{
	public:

		Fruit();
		~Fruit();
		bool UpdateObject(float dt) override;
		int  getScore();
	protected:
		bool OnCollision(SimObject* obj);
		int score = 1000;
	};
}
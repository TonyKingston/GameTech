#pragma once
#include "SimObject.h"

/*
	Author: Tony Kingston
	File: StaticObject.h
	Last Updated: 22/5/2021

	Description:
	     Header for the StaticObject class. This class represents a static rigid body e.g. a section of wall or ground

*/

namespace NCL::CSC3222 {
	class StaticObject : public SimObject {
	public:
		StaticObject(bool climable);
		~StaticObject();
		bool UpdateObject(float dt) override;
		void setClimable(bool isClimable) {
			climable = isClimable;
		}

		bool isClimable() {
			return climable;
		}
	protected:
		bool climable;
		bool OnCollision(SimObject* obj);
	};
}

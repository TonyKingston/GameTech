#pragma once
#include "SimObject.h"
#include "GameMap.h"
#include "PixieDust.h"
/*
	Modified by Tony Kingston
	File: Froggo.h
	Last Updated: 22/5/2021

*/

namespace NCL::CSC3222 {
	class Froggo : public SimObject {
	public:

		enum class State {
			ATTACK,
			GUARD,
			STUNNED
		};

		Froggo();
		~Froggo();

		bool UpdateObject(float dt) override;
		vector<MapNode*> getCurrentPath() {
			return currentPath;
		}

		State getCurrentState() const {
			return currentState;
		}

	/*	void setTarget(PixieDust dust) {
			target = dust;
		}*/
		bool  isStunned;
		float stunTimer;

	protected:
		bool OnCollision(SimObject* obj);
		void AttackPlayer();
		void GuardDust(PixieDust* target);
		float timer;
		State currentState;
		vector<MapNode*> currentPath;
		MapNode* startNode;
		MapNode* destNode;
		int pathSteps;
		bool newPath;
		float nodeThreshold = 8.0f;
		float nodeTimer;
		
	};
}
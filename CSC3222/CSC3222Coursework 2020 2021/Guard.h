#pragma once
#include "SimObject.h"

/*
	Modified by Tony Kingston
	File: Guard.h
	Last Updated: 22/5/2021

*/

namespace NCL::CSC3222 {
	class Guard : public SimObject	{
	public:

		enum class State {
			PATROL,
			ATTACK,
			STUNNED,
			MOVERIGHT,
			MOVELEFT
		};
		Guard();
		~Guard(); 

		/*void SetPosition(const Vector2& newPosition) {
			position = newPosition;
			patrolPoint = position;
		}*/
		bool UpdateObject(float dt) override;
		void setPatrolPoint(Vector2 point);
	    bool  isStunned;
		float stunTimer;
		State getState() const {
			return currentState;
		}

		void Stun();

	protected:
		bool OnCollision(SimObject* obj) override;
		//void PatrolArea();
		void AttackPlayer();
		void MoveRight();
		void MoveLeft();
		float timer;
		int x;
		int y;
		GameMap* map;
		
		Vector2 patrolPoint;
		Vector2 playerPos;
		Vector4* animSource;
		State currentState;
		
	};
}
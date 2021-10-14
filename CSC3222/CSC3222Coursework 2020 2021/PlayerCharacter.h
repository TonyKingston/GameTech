#pragma once
#include "SimObject.h"

/*
	Modified by Tony Kingston
	File: PlayerCharacter.h
	Last Updated: 22/5/2021

*/

namespace NCL::CSC3222 {
	class PlayerCharacter : public SimObject	{
	public:
		PlayerCharacter();
		~PlayerCharacter();

		bool UpdateObject(float dt) override;
		int getMagicCount();
		void setMagicCount(int count);
		void setOnLadder(bool isOnLadder) {
			onLadder = isOnLadder;
		}
		bool pixieMode;
		bool onLadder;

	protected:
		bool OnCollision(SimObject* x);
		enum class PlayerState {
			Left,
			Right,
			Attack,
			Fall,
			Die,
			Idle
		};
		PlayerState		currentAnimState;
		int          magicCount;
	};
}
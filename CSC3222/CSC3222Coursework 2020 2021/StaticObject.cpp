#include "StaticObject.h"
#include "Spell.h"
#include "PlayerCharacter.h"
#include "Guard.h"

/*
	Author: Tony Kingston
	File: StaticObject.cpp
	Last Updated: 22/5/2021


*/

using namespace NCL;
using namespace CSC3222;

StaticObject::StaticObject(bool bClimable) : SimObject() {
	inverseMass = 0;
	type = OBSTACLE;
	maxVelocity = 0.0f;
	mIsStatic = true;
	climable = bClimable;
	elasticity = 1;
	if (bClimable) {
		type = LADDER;
	}
	else {
		type = OBSTACLE;
	}
	
}

StaticObject::~StaticObject() {
}

bool StaticObject::UpdateObject(float dt) {
	return isAlive;
}

bool StaticObject::OnCollision(SimObject* obj) {
	ObjType type = obj->type;
	if (type == PIXIE || type == FRUIT || type == DUST) {
		return false;
	}
	else if (type == SPELL && !climable) {
		Spell* spell = (Spell*)obj;
		spell->addBounce();
		return true;
	}
	else {
		if (climable) {
			if (type == PLAYER) {
				PlayerCharacter* player = game->getPlayer();
				player->setOnLadder(true);
				player->AddForce(-gravity);
			}
			else if (type == GUARD) {
				Guard* guard = (Guard*)obj;
				guard->AddForce(-gravity);
			}
			return false;
		}
		return true;
	}
}
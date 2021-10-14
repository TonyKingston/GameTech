#include "Guard.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "AABB.h"
#include "Spell.h"
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Maths.h"
#include "SimObject.h"
#include "PlayerCharacter.h"
#include "GameMap.h"
#include "StaticObject.h"
/*
	Modified by Tony Kingston
	File: Guard.cpp
	Last Updated: 22/5/2021


*/

using namespace NCL;
using namespace CSC3222;

Vector4 guardIdleFrames[] = {
	Vector4(384,32, 64, 32),
	Vector4(448,32, 64, 32),	
	Vector4(512,32, 64, 32),	
	Vector4(576,32, 64, 32),	
	Vector4(448,64, 64, 32),	
	Vector4(512,64, 64, 32),
};

Vector4 guardWalkFrames[] = {
	Vector4(384,96, 64, 32),
	Vector4(448,96, 64, 32),
	Vector4(512,96, 64, 32),
	Vector4(576,96, 64, 32),
	Vector4(640,96, 64, 32),
	Vector4(704,96, 64, 32),
};

Vector4 guardAttackFrames[] = {
	Vector4(384,128, 64, 32),
	Vector4(448,128, 64, 32),
	Vector4(512,128, 64, 32),
	Vector4(576,128, 64, 32),
	Vector4(640,128, 64, 32),
	Vector4(704,128, 64, 32),
};

Vector4 guardStunFrames[] = {
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
};

Guard::Guard() : SimObject() {
	texture = texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	animFrameCount	= 6;
	inverseMass = 0.5f;
	damping = 0.998f;
	type = GUARD;
	currentState = State::MOVERIGHT;
	elasticity = 0.4f;
	
	//AABB* collider = new AABB(GetPosition(), 10, 16);
	Circle* collider = new Circle(GetPosition(), 16);
	this->SetCollider(collider);

	map = game->getCurrentMap();
}

Guard::~Guard() {
}

bool Guard::UpdateObject(float dt) {
	animSource = guardIdleFrames;
	timer += dt;

	Vector2 mapPos = map->WorldPosToTilePos(position);
	x = mapPos.x;
	y = mapPos.y;

	playerPos = game->getPlayer()->GetPosition();

	switch (currentState) {
	case State::MOVERIGHT:
		MoveRight();
		break;
	case State::MOVELEFT:
		MoveLeft();
		break;
	case State::ATTACK:
		AttackPlayer();
		break;
	case State::STUNNED:
		stunTimer += dt;
		animSource = guardStunFrames;
		if (stunTimer >= 5) {
			stunTimer = 0;
			currentState = State::MOVERIGHT;
			// New patrol point if knocked off platform. If the knockback caused them to fall, it would seem resonable for them to have settled on the ground after 5s
			if (position.y <= patrolPoint.y - 16) {
				patrolPoint = position;
			}
		}
		break;
	}
	
	animFrameData = animSource[currentanimFrame];
	AddForce(gravity);
	return isAlive;
}

bool Guard::OnCollision(SimObject* obj) {
	ObjType type = obj->type;
	if (type == PLAYER && !isStunned) {
		return obj->OnCollision(this);
		/*game->playerStart();
		game->setLives(game->getLives() - 1);
		return false;*/
	}
	else if (type == SPELL) {
		return obj->OnCollision(this);
		/*Spell* spell = (Spell*)obj;
		Stun();
		spell->addBounce();
		game->addScore(200);
		return true;*/
	}
	else if (type == OBSTACLE) {
		return true;
	}
	else if (type == LADDER) {
		AddForce(-gravity);
		return false;
	}

	return false;
}

/*void Guard::PatrolArea() {
//	std::cout << "patrolling" << std::endl;
	Vector4* animSource = guardWalkFrames;
	animFrameData = animSource[currentanimFrame];
	PlayerCharacter* player = (PlayerCharacter*)game->getPlayer();
	flipAnimFrame = true;
	if (position.x < (patrolPoint.x + 160)) {
		if (player->GetPosition().x < position.x + 48 && player->GetPosition().y >= position.y) {
			currentState = State::ATTACK;
			return;
		}
		AddForce(Vector2(32, 0));
		animSource = guardWalkFrames;
		return;
	}
	flipAnimFrame = false;
	if (position.x > (patrolPoint.x - 160)) {
		if (player->GetPosition().x < position.x - 48 && player->GetPosition().y >= position.y) {
			currentState = State::ATTACK;
			return;
		}
		AddForce(Vector2(-32, 0));
		animSource = guardWalkFrames;
		return;
	}
	AddForce((patrolPoint - position).Normalised() * 32);
}*/

void Guard::MoveRight() {
	//std::cout << "move right" << std::endl;
	animSource = guardWalkFrames;
	//animFrameData = animSource[currentanimFrame];
	PlayerCharacter* player = (PlayerCharacter*)game->getPlayer();
	flipAnimFrame = true;
	char nextTile = map->getMapData()[((y - 1) * map->GetMapWidth()) + x + 1];
	//std::cout << map->getMapData()[((y) * map->GetMapWidth()) + x + 1] << std::endl;
	float distance = (playerPos - position).Length();
	if (position.x < (patrolPoint.x + 160) && nextTile != '0' && map->getMapData()[(y * map->GetMapWidth()) + x + 2] != 'W') {
		if (distance <= 48) {
			currentState = State::ATTACK;
			return;
		}
		AddForce(Vector2(32, 0));
		animSource = guardWalkFrames;
		return;
	}
	//SetVelocity(Vector2(0, 0));
	AddImpulse(-velocity);
	currentState = State::MOVELEFT;
}

void Guard::MoveLeft() {
	//std::cout << "move left" << std::endl;
	animSource = guardWalkFrames;
	PlayerCharacter* player = (PlayerCharacter*)game->getPlayer();
	flipAnimFrame = false;
	//animFrameData = animSource[currentanimFrame];
	char nextTile = map->getMapData()[((y - 1) * map->GetMapWidth()) + x - 1];
	//std::cout << map->getMapData()[((y - 1) * map->GetMapWidth()) + x - 1] << std::endl;
	float distance = (playerPos - position).Length();
	if (position.x > (patrolPoint.x - 160) && nextTile != '0' && map->getMapData()[(y * map->GetMapWidth()) + x - 1] != 'W') {
		if (distance <= 48) {
			currentState = State::ATTACK;
			return;
		}
		AddForce(Vector2(-32, 0));
	    animSource = guardWalkFrames;
		return;
	}
	AddImpulse(-velocity);
	currentState = State::MOVERIGHT;
}

void Guard::AttackPlayer() {
	std::cout << "attacking" << std::endl;
	animSource = guardAttackFrames;
	float distance = (playerPos - position).Length();
	char nextTileLeft = map->getMapData()[((y - 1) * map->GetMapWidth()) + x - 1];
	char nextTileRight = map->getMapData()[((y - 1) * map->GetMapWidth()) + x - 1];
	if (distance > 80 || nextTileLeft == '0' || nextTileRight == '0') {
		currentState = State::MOVELEFT;
		return;
	}
	AddForce((playerPos - position).Normalised() * 32);


}

void Guard::setPatrolPoint(Vector2 point) {
	patrolPoint = point;
}

void Guard::Stun() {
	currentState = State::STUNNED;
}
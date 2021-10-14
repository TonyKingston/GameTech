#include "Froggo.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "Circle.h"
#include "Spell.h"
#include "../../Common/Maths.h"
#include "PlayerCharacter.h"
#include "PixieDust.h"
#include "StaticObject.h"
/*
	Modified by Tony Kingston
	File: Froggo.cpp
	Last Updated: 22/5/2021


*/

using namespace NCL;
using namespace CSC3222;

Vector4 froggoIdleFrames[] = {
	Vector4(0  ,(96 * 5) + 32, 80, 64),
	Vector4(80 ,(96 * 5) + 32, 80, 64),
	Vector4(160,(96 * 5) + 32, 80, 64),
	Vector4(240,(96 * 5) + 32, 80, 64),
	Vector4(320,(96 * 5) + 32, 80, 64),
	Vector4(400,(96 * 5) + 32, 80, 64),
};

Vector4 froggoWalkFrames[] = {
	Vector4(0  ,(96 * 5) + 32, 80, 64),
	Vector4(80 ,(96 * 5) + 32, 80, 64),
	Vector4(160,(96 * 5) + 32, 80, 64),
	Vector4(240,(96 * 5) + 32, 80, 64),
	Vector4(320,(96 * 5) + 32, 80, 64),
	Vector4(400,(96 * 5) + 32, 80, 64),
};

Vector4 froggoLadderFrames[] = {
	Vector4(0  ,(96 * 4) + 32, 80, 64),
	Vector4(80 ,(96 * 4) + 32, 80, 64),
	Vector4(160,(96 * 4) + 32, 80, 64),
	Vector4(240,(96 * 4) + 32, 80, 64),
	Vector4(320,(96 * 4) + 32, 80, 64),
	Vector4(400,(96 * 4) + 32, 80, 64),
};

Vector4 froggoStunFrames[] = {
	Vector4(0  ,(96 * 8) + 32, 80, 64),
	Vector4(80, (96 * 8) + 32, 80, 64),
	Vector4(160,(96 * 8) + 32, 80, 64),
	Vector4(240,(96 * 8) + 32, 80, 64),
	Vector4(320,(96 * 8) + 32, 80, 64),
	Vector4(400,(96 * 8) + 32, 80, 64),
};

Vector4 froggoAttackFrames[] = {
	Vector4(0  ,(96 * 1) + 32, 80, 64),
	Vector4(80 ,(96 * 1) + 32, 80, 64),
	Vector4(80 ,(96 * 1) + 32, 80, 64),
	Vector4(160,(96 * 1) + 32, 80, 64),
	Vector4(160,(96 * 1) + 32, 80, 64),
	Vector4(240,(96 * 1) + 32, 80, 64),
};

Vector4 froggoJumpFrames[] = {
	Vector4(0  ,(96 * 7) + 32, 80, 64),
	Vector4(80 ,(96 * 7) + 32, 80, 64),
	Vector4(80 ,(96 * 7) + 32, 80, 64),
	Vector4(160,(96 * 7) + 32, 80, 64),
	Vector4(160,(96 * 7) + 32, 80, 64),
	Vector4(240,(96 * 7) + 32, 80, 64),
};

Froggo::Froggo() : SimObject() {
	texture			 = texManager->GetTexture("FruitWizard\\frogman_crown.png");
	animFrameCount	 = 6;
	inverseMass = 0.5f;
	damping = 0.998f;
	timer = 0.0f;
	offset = Vector2(0, 2);
	elasticity = 0.4;

	type = FROGGO;
	currentState = State::ATTACK;
	Circle* collider = new Circle(GetPosition() + offset, 14);
	this->SetCollider(collider);
	newPath = true;

}

Froggo::~Froggo() {
	currentPath.clear();
}

bool Froggo::UpdateObject(float dt) {
	Vector4* animSource = froggoIdleFrames;
	timer += dt;
	nodeTimer += dt;
	switch (currentState) {
	case State::ATTACK:
		if (timer > 10) {
			newPath = true;
		}
		animSource = froggoWalkFrames;
		AttackPlayer();
		break;
	case State::GUARD:
		animSource = froggoWalkFrames;
		GuardDust(game->getPixieDust()[0]);
		break;
	case State::STUNNED:
		stunTimer += dt;
		animSource = froggoStunFrames;
		if (stunTimer >= 3) {
			stunTimer = 0;
			//collider = NULL;
			currentState = State::ATTACK;
		}
		break;
	}
	if (!currentPath.empty() && pathSteps < currentPath.size()) {
		MapNode* currentNode = currentPath.at(currentPath.size() - pathSteps -	1);
		float distance = Vector2((currentNode->posX + 1) * 16 + 16 - position.x, (currentNode->posY + 1) * 16 + 16 - position.y).Length();

		if (pathSteps < currentPath.size() - 1) {
			MapNode* nextNode = currentPath.at(currentPath.size() - pathSteps - 2);
			SetPosition(Vector2((nextNode->posX + 1) * 16, (nextNode->posY + 1) * 16));
			Vector2 direction = Vector2((nextNode->posX + 1) * 16 - position.x, (nextNode->posY + 1) * 16 - position.y).Normalised();
			//AddForce(direction * 64);
			//pathSteps += 1;
		}
		else {
			pathSteps = 0;
			currentPath.clear();
		}

		if (nodeTimer >= 0.2) {
			pathSteps += 1;
			nodeTimer = 0;

		}
		
	}

	animFrameData = animSource[currentanimFrame];
	AddForce(gravity);

	return isAlive;
}

bool Froggo::OnCollision(SimObject* obj) {
	ObjType type = obj->type;
	if (type == PLAYER && !isStunned) {
		/*game->playerStart();
		game->setLives(game->getLives() - 1);
		return false;*/
		return obj->OnCollision(this);
	}
	else if (type == SPELL) {
		obj->OnCollision(this);
		/*Spell* spell = (Spell*)obj;
		spell->addBounce();
		isStunned = true;
		return true;*/
	}
	else if (type == OBSTACLE) {
		return true;
	}
	else if (type == LADDER) {
		return false;
	}
	return false;
}

void Froggo::AttackPlayer() {
	std::cout << "attack" << std::endl;
	if (newPath) {
		timer = 0;
		pathSteps = 0;
		GameMap* cMap = game->getCurrentMap();
		Vector2 startPos = cMap->WorldPosToTilePos(position);
		Vector2 destPos = cMap->WorldPosToTilePos(game->getPlayer()->GetPosition());
		startNode = &cMap->nodeData[(int)startPos.y * cMap->GetMapWidth() + (int)startPos.x];
		destNode = &cMap->nodeData[(int)destPos.y * cMap->GetMapWidth() + (int)destPos.x];
		currentPath = cMap->aStar(startNode, destNode);
		newPath = false;
	}
	if (!game->getPixieDust().empty() && game->getDustCount() == 0) {
		currentState = State::GUARD;
	//	pathSteps = 0;
		newPath = true;
	}
	
}

void Froggo::GuardDust(PixieDust* target) {
	std::cout << "guard" << std::endl;
	if (target == nullptr) {
		currentState = State::ATTACK;
		newPath = true;
		return;
	}
	else if (newPath) {
		timer = 0;
		pathSteps = 0;
		GameMap* cMap = game->getCurrentMap();
		Vector2 startPos = cMap->WorldPosToTilePos(position);
		Vector2 destPos = cMap->WorldPosToTilePos(target->GetPosition());
		startNode = &cMap->nodeData[(int)startPos.y * cMap->GetMapWidth() + (int)startPos.x];
		destNode = &cMap->nodeData[(int)destPos.y * cMap->GetMapWidth() + (int)destPos.x];
		currentPath = cMap->aStar(startNode, destNode);
		newPath = false;
		return;
	}
	//currentState = State::ATTACK;
	//newPath = true;
	/*if (currentPath.empty()) {
		currentState = State::ATTACK;
		newPath = true;
	}*/
	
}
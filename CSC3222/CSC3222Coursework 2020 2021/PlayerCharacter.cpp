#include "PlayerCharacter.h"
#include "TextureManager.h"
#include "../../Common/Window.h"
#include "GameSimsRenderer.h"
#include <iostream>
#include <random>
#include "Spell.h"
#include "Circle.h"
#include "StaticObject.h"

/*
	Modified by Tony Kingston
	File: PlayerCharacter.cpp
	Last Updated: 22/5/2021


*/

using namespace NCL;
using namespace CSC3222;

Vector4 runFrames[] = {
	Vector4(64,160, 32, 32),
	Vector4(96,160, 32, 32),
	Vector4(128,160, 32, 32),
	Vector4(160,160, 32, 32),
	Vector4(192,160, 32, 32),
	Vector4(224,160, 32, 32),
};

Vector4 attackFrames[] = {
	Vector4(128,288, 32, 30),
	Vector4(64,224, 32, 32),
	Vector4(160,288, 32, 30),
	Vector4(96,224, 32, 32),
	Vector4(192,288, 32, 29),
	Vector4(64,256, 32, 32)
};

Vector4 idleFrames[] = {
	Vector4(64,128, 32, 32),
	Vector4(96,128, 32, 32),
	Vector4(128,128, 32, 32),
	Vector4(160,128, 32, 32),
	Vector4(128,128, 32, 32),
	Vector4(224,128, 32, 32)
};

Vector4 fallFrames[] = {
	Vector4(64,320, 32, 32),
	Vector4(64,320, 32, 32),
	Vector4(64,320, 32, 32),
	Vector4(96,320, 32, 32),
	Vector4(96,320, 32, 32),
	Vector4(96,320, 32, 32)
};

Vector4 deathFrames[] = {
	Vector4(96,352, 32, 32),
	Vector4(128,352, 32, 32),
	Vector4(96,352, 32, 32),	
	Vector4(128,352, 32, 32),
	Vector4(96,352, 32, 32),
	Vector4(128,352, 32, 32),
};

Vector4 ladderFrames[] = {//Not an ideal spritesheet for ladders!
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
};


PlayerCharacter::PlayerCharacter() : SimObject() {
	currentAnimState	= PlayerState::Left;
	texture				= texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	animFrameCount		= 6;
	damping = 0.998;
	inverseMass = 0.2;
	magicCount = 4;
	type = PLAYER;
	offset = Vector2(0, -4);
	pixieMode = false;
	elasticity = 0.1f;

    Circle* collider = new Circle(GetPosition() + offset, 12.0f);
	//AABB* collider = new AABB(GetPosition(), 10.0f);
	this->SetCollider(collider);
}

PlayerCharacter::~PlayerCharacter() {

}

void PlayerCharacter::setMagicCount(int count) {
	magicCount = count;
}

bool PlayerCharacter::UpdateObject(float dt) {

	float testSpeed = 64;
	Vector4* animSource = idleFrames;
	Vector2 newVelocity;
	float constForce = 256;
	Vector2 newForce;

	if (currentAnimState == PlayerState::Attack) {
		animSource = attackFrames;
		if (currentanimFrame >= 5) {
			currentAnimState = PlayerState::Idle;
		}
	}
	else {
		currentAnimState = PlayerState::Idle;
		
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			animSource = runFrames;
			currentAnimState = PlayerState::Left;
			//newVelocity.x = -testSpeed * dt;
			newForce = Vector2(-constForce, 0);
			flipAnimFrame = true;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			animSource = runFrames;
			currentAnimState = PlayerState::Right;
			//newVelocity.x = testSpeed * dt;
			newForce = Vector2(constForce, 0);
			flipAnimFrame = false;
		}	
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && magicCount > 0) {
			std::random_device rd; // obtain a random number from hardware
			std::mt19937 gen(rd()); // seed the generator
			std::uniform_int_distribution<> ydistr(-40, 40);
			float xForce = 0;
			if (flipAnimFrame == false) {
				xForce = 128;
			}
			else {
				xForce = -128;
			}
			
			currentAnimState = PlayerState::Attack;
			currentanimFrame = 0;
			Spell* newSpell = new Spell(Vector2(xForce, ydistr(gen)));
			//Spell* newSpell = new Spell(Vector2(xForce, 0));
			newSpell->SetPosition(GetPosition() + Vector2(15,0));
			newSpell->AddImpulse(Vector2(xForce, ydistr(gen)));
			game->AddNewObject(newSpell);
			magicCount -= 1;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP) && onLadder) {
			animSource = runFrames;
			currentAnimState = PlayerState::Idle;
			newForce = Vector2(0, constForce / 2);
			flipAnimFrame = false;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN) && onLadder) {
			animSource = runFrames;
			currentAnimState = PlayerState::Idle;
			newForce = Vector2(0, -constForce / 2);
			flipAnimFrame = false;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::CONTROL) && game->getDustCount() == 4) {
			pixieMode = true;
		}
	}

	/*if (currentAnimState == PlayerState::Idle) {
		this->SetVelocity(Vector2(0, 0));
	}*/
	AddForce(newForce + gravity);
	//position += newVelocity;
	animFrameData = animSource[currentanimFrame];
	onLadder = false;
	return true;
}

int PlayerCharacter::getMagicCount() {
	return magicCount;
}

bool PlayerCharacter::OnCollision(SimObject* b) {
	ObjType type = b->type;
	if (type == FRUIT) {
		game->addScore(1000);
		b->setStatus(false);
		game->incrementFruitCount();
		return false;
	}
	else if (type == DUST) {
		game->addScore(300);
		b->setStatus(false);
		game->addDustCount();
		return false;
	}
	else if (type == GUARD) {
		game->playerStart();
		game->setLives(game->getLives() - 1);
		return false;
	}
	else if (type == FROGGO) {
		game->playerStart();
		game->setLives(game->getLives() - 1);
		return false;
	}
	else if (type == OBSTACLE) {
		return true;
	}
	else if (type == LADDER) {
		onLadder = true;
		 AddForce(-gravity);
		return false;
	}

	return false;
}

#include "PixieDust.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "Circle.h"
#include "PlayerCharacter.h"

/*
	Modified by Tony Kingston
	File: PixieDust.cpp
	Last Updated: 22/5/2021


*/

using namespace NCL;
using namespace CSC3222;

Vector4 animFrames[] = {
	Vector4(64 , 192, 32, 32),
	Vector4(96, 192, 32, 32),
	Vector4(128, 192, 32, 32),
	Vector4(160, 192, 32, 32)
};

PixieDust::PixieDust() : SimObject() {
	animFrameCount = 4;
	texture = texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");

	type = DUST;
	Circle* collider = new Circle(GetPosition(), 4);
	this->SetCollider(collider);
}

PixieDust::~PixieDust() {
}

bool PixieDust::UpdateObject(float dt) {
	animFrameData = animFrames[currentanimFrame];

	return isAlive;
}

bool PixieDust::OnCollision(SimObject* obj) {
	ObjType type = obj->type;
	if (type == PLAYER) {
		return obj->OnCollision(this);
		/*game->addScore(300);
		game->addDustCount();
		setStatus(false);*/
	}
	return false;
}
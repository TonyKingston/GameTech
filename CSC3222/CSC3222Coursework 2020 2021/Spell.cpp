#include "Spell.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"

#include "../../Common/Maths.h"

#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "Circle.h"
#include "Guard.h"
#include "Froggo.h"

/*
	Modified by Tony Kingston
	File: Spell.cpp
	Last Updated: 22/5/2021


*/

using namespace NCL;
using namespace CSC3222;

Vector4 activeFrames[] = {
	Vector4(512,384, 32, 32),
	Vector4(512,384, 32, 32),
	Vector4(512,384, 32, 32),
	Vector4(384,416, 32, 32),
	Vector4(384,416, 32, 32),
	Vector4(384,416, 32, 32),
};

Vector4 explodeFrames[] = {
	Vector4(512,384, 32, 32),
	Vector4(384,416, 32, 32),
	Vector4(416,416, 32, 32),
	Vector4(448,416, 32, 32),
	Vector4(480,416, 32, 32),
	Vector4(512,416, 32, 32),	
};

Spell::Spell(Vector2 direction) : SimObject()	{
	texture		= texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png"); // Instant change in velocity
	animFrameCount = 6;
	damping = 1.0f;
	inverseMass = 5.0f;
	timer = 0.0f;
	maxVelocity = 128.0f;
	elasticity = 0.8f;

	type = SPELL;
	Circle* collider = new Circle(GetPosition(), 7);
	this->SetCollider(collider);
}

Spell::~Spell()	{
}

void Spell::DrawObject(GameSimsRenderer &r) {
	Vector4	texData = activeFrames[currentanimFrame];
	Vector2 texPos	= Vector2(texData.x, texData.y);
	Vector2 texSize = Vector2(texData.z, texData.w);
	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position);
}

bool Spell::UpdateObject(float dt) {
	timer += dt;
	if (bounceCount >= 3) {
		isAlive = false;
	}
//	std::cout << velocity.x << std::endl;
	animFrameData = activeFrames[currentanimFrame];
	return isAlive;
}

bool Spell::OnCollision(SimObject* b) {
	ObjType type = b->type;
	if (type == GUARD) {
		Guard* guard = (Guard*)b;
		guard->Stun();
		bounceCount += 1;
		game->addScore(200);
		return true;
	}
	else if (type == FROGGO) {
		Froggo* frog = (Froggo*)b;
		frog->isStunned = true;
		bounceCount += 1;
		return true;
	}
	else if (type == OBSTACLE) {
		bounceCount += 1;
		return true;
	}

	return false;
}

void Spell::addBounce() {
	bounceCount += 1;
}
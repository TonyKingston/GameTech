#include "SimObject.h"
#include "CollisionVolume.h"
#include "GameSimsRenderer.h"

/*
	Modified by Tony Kingston
	File: SimObject.cpp
	Last Updated: 22/5/2021


*/

using namespace NCL;
using namespace CSC3222;

TextureManager*  SimObject::texManager	= nullptr;
FruitWizardGame* SimObject::game		= nullptr;

SimObject::SimObject()	{
	texture		= nullptr;
	collider	= nullptr;
	offset = Vector2(0, 0);

	animFrameCount		= 1;
	currentanimFrame	= 0;
	frameTime			= 0;
	frameRate			= 10;
	flipAnimFrame = true;
}

SimObject::~SimObject()	{
	delete collider;
}

void SimObject::SetCollider(CollisionVolume* c) {
	collider = c;
	collider->setObject(this);
}

CollisionVolume* SimObject::GetCollider() const{
	return collider;
}

void SimObject::InitObjects(FruitWizardGame* game, TextureManager* texManager) {
	SimObject::game			= game;
	SimObject::texManager	= texManager;
}

void SimObject::UpdateAnimFrame(float dt) {
	frameTime -= dt;

	if (frameTime < 0) {
		currentanimFrame = (currentanimFrame + 1) % animFrameCount;
		frameTime += 1.0f / frameRate;
	}
}

void SimObject::DrawObject(GameSimsRenderer& r) {
	Vector2 texPos		= Vector2(animFrameData.x, animFrameData.y);
	Vector2 texSize		= Vector2(animFrameData.z, animFrameData.w);
	Vector2 screenPos	= position;

	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position, flipAnimFrame);
}

Vector2 SimObject::getOffset() {
	return offset;
}

const char* SimObject::getType() {
	return objTypes[type];
}

void SimObject::setStatus(bool alive) {
	isAlive = alive;
}

bool SimObject::OnCollision(SimObject* x) {
	return true;
}
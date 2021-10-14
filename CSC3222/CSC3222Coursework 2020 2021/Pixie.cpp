#include "Pixie.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "AABB.h"
#include "PlayerCharacter.h"
#include "Guard.h"

/*
    Modified by Tony Kingston
	File: Pixie.cpp
	Last Updated: 22/5/2021


*/

using namespace NCL;
using namespace CSC3222;

Vector4 pixieFrames[] = {
	Vector4(32 * 1,32 * 4, 32, 32),
	Vector4(32 * 2,32 * 4, 32, 32),
	Vector4(32 * 3,32 * 4, 32, 32),
	Vector4(32 * 4,32 * 4, 32, 32),
};

Pixie::Pixie() : SimObject() {
	texture			= texManager->GetTexture("FruitWizard\\super_random_sprites.png");
	animFrameCount	= 4;
	inverseMass = 2.0f;
	damping = 0.990f;
	elasticity = -8.0f;
	attached = false;
	type = PIXIE;
//	AABB* collider = new AABB(GetPosition(), 8, 9);
	//this->SetCollider(collider);
}

Pixie::~Pixie() {
}

bool Pixie::UpdateObject(float dt) {
	float dotProduct = Vector2::Dot(position.Normalised(), velocity);


	animFrameData = pixieFrames[currentanimFrame];
	// If the pixie if attached to the player, apply a force based on Hooke's Law
	if (attached) {
		AddForce(Vector2(elasticity * (GetPosition().x - game->getPlayer()->GetPosition().x + 15) - (dotProduct * 1.0f),
			elasticity * (GetPosition().y - game->getPlayer()->GetPosition().y - 10) - (dotProduct * 1.0f)));
	}
	float distance = (game->getPlayer()->GetPosition() - position).Length();
	AddForce((game->getPlayer()->GetPosition() - position).Normalised());

	Flocking(dt);

	if (distance <= 5.0f) {
		PlayerCharacter* player = game->getPlayer();
		game->addScore(1000);
		player->setMagicCount(player->getMagicCount() + 1);
		isAlive = false;
	}

	return isAlive;
}

void Pixie::attachToPlayer() {
	attached = true;
}

bool Pixie::OnCollision(SimObject* obj) {
	ObjType type = obj->type;
	if (type == PLAYER) {
		PlayerCharacter* player = game->getPlayer();
		game->addScore(1000);
		player->setMagicCount(player->getMagicCount() + 1);
		setStatus(false);
	}
	return false;
}

void Pixie::Flocking(float dt) {
	for (Pixie* pixie : game->getPixies()) {
		Vector2 dir;
		dir += Alignment();
		dir += Separation();
		dir += Cohesion();
		dir += Avoidance();
		pixie->position += dir * dt;
	}
}

Vector2 Pixie::Alignment() {
	Vector2 direction = velocity.Normalised();
	for (Pixie* pixie : game->getPixies()) {
		if (pixie == this) {
			continue;
		}
		float distance = (position - pixie->position).Length();
		if (distance > 16.0f) {
			continue;
		}
		direction += pixie->GetVelocity().Normalised();
	}
	return direction.Normalised();
}

Vector2 Pixie::Separation() {
	Vector2 direction = velocity;
	for (Pixie* pixie : game->getPixies()) {
		if (pixie == this) {
			continue;
		}
		float distance = (position - pixie->position).Length();
		if (distance > 20.0f) {
			continue;
		}
		float strength = 1.0f - (distance / 20);
		if (distance <= 5.0f) {
			strength *= 10;
		}
		direction += (position - pixie->position).Normalised() * strength;
	}
	return direction.Normalised();
}

Vector2 Pixie::Cohesion() {
	Vector2 avgPos = position;
	float count = 1;
	for (Pixie* pixie : game->getPixies()) {
		if (pixie == this) {
			continue;
		}
		float distance = (position - pixie->position).Length();
		if (distance > 16.0f) {
			continue;
		}
		avgPos += pixie->position;
		count += 1;
	}
	avgPos = avgPos / count;
	return (avgPos - position).Normalised();
}

Vector2 Pixie::Avoidance() {
	Vector2 direction;
	for (Guard* guard : game->getGuards()) {
		float distance = (position - guard->GetPosition()).Length();
		if (distance < 32.0f) {
			//AddForce((position - guard->GetPosition()).Normalised());
			direction += (position - guard->GetPosition()).Normalised();
		}
	}
	return direction.Normalised();
}
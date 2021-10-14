#include "Fruit.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "Circle.h"

/*
	Modified by Tony Kingston
	File: Fruit.cpp
	Last Updated: 22/5/2021


*/

using namespace NCL;
using namespace CSC3222;

Fruit::Fruit() : SimObject() {
	texture = texManager->GetTexture("FruitWizard\\Fruit.png");

	int fruitID = rand() % 16;

	animFrameData = Vector4((fruitID / 4) * 16.0f, (fruitID % 4) * 16.0f, 16.0f, 16.0f);

	type = FRUIT;
	Circle* collider = new Circle(GetPosition(), 4);
	this->SetCollider(collider);
}

Fruit::~Fruit() {
}

bool Fruit::UpdateObject(float dt) {
	return isAlive;
}

int Fruit::getScore() {
	return score;
}

bool Fruit::OnCollision(SimObject* obj) {
	ObjType type = obj->type;
	if (type == PLAYER) {
		return obj->OnCollision(this);
		/*game->addScore(1000);
		game->incrementFruitCount();
	    setStatus(false);*/
	}
	return false;
}
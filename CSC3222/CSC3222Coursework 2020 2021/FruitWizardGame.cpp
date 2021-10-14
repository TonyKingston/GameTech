#include "FruitWizardGame.h"
#include "SimObject.h"
#include "GameMap.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "GameSimsPhysics.h"
#include "PlayerCharacter.h"
#include "Fruit.h"
#include "Guard.h"
#include "Froggo.h"
#include "Pixie.h"
#include "PixieDust.h"
#include "Spell.h"

#include "../../Common/Window.h"
#include "../../Common/TextureLoader.h"
#include "AABB.h"
#include <random>

/*
	Modified by Tony Kingston
	File: FruitWizardGame.cpp
	Last Updated: 22/5/2021

*/

using namespace NCL;
using namespace CSC3222;

FruitWizardGame::FruitWizardGame() {
	renderer = new GameSimsRenderer();
	texManager = new TextureManager();
	physics = new GameSimsPhysics();
	SimObject::InitObjects(this, texManager);
	InitialiseGame();
}

FruitWizardGame::~FruitWizardGame() {
	delete currentMap;
	delete texManager;
	delete renderer;
	delete physics;
}

void FruitWizardGame::Update(float dt) {

	for (auto i : newObjects) {
		gameObjects.emplace_back(i);
	}
	newObjects.clear();

	gameTime += dt;
	dustTime += dt;

	renderer->Update(dt);
	physics->Update(dt);
	currentMap->DrawMap(*renderer);

	srand((int)(gameTime * 1000.0f));

	for (auto i = gameObjects.begin(); i != gameObjects.end(); ) {
		(*i)->UpdateAnimFrame(dt);
		DrawColliders(*i);
			if (!(*i)->UpdateObject(dt)) { //object has said its finished with
				physics->RemoveCollider((*i)->GetCollider());
				physics->RemoveRigidBody((*i));
				delete (*i);
				i = gameObjects.erase(i);
			}
			else {
				(*i)->DrawObject(*renderer);
				++i;
			}
	}
	renderer->DrawString("Score:" + std::to_string(currentScore),
		Vector2(32, 12), Vector4(1, 1, 1, 1), 100.0f);
	renderer->DrawString("Lives:" + std::to_string(lives),
		Vector2(144, 12), Vector4(1, 0, 0, 1), 100.0f);
	renderer->DrawString("Magic:" + std::to_string(player->getMagicCount()),
		Vector2(256, 12), Vector4(1, 0, 0.5f, 1), 100.0f);
	renderer->DrawString("Dust:" + std::to_string(dustCount),
		Vector2(366, 12), Vector4(0.5f, 0.3f, 0.8f, 1), 100.0f);

	/*
	Some examples of debug rendering!
	These all use coordinates in the same 'space' as the game positions
	*/
	//renderer->DrawBox(Vector2(16, 16), Vector2(8, 8), Vector4(1, 0, 0, 1));
	//renderer->DrawLine(Vector2(16, 16), Vector2(192, 192), Vector4(1, 1, 0, 1));
	//renderer->DrawCircle(Vector2(100, 100), 10.0f, Vector4(1, 0, 1, 1));

	renderer->DrawCircle(player->GetPosition() + player->getOffset(), 12.0f, Vector4(1, 0, 1, 1));
//	renderer->DrawBox(player->GetPosition() + player->getOffset(), Vector2(10, 10), Vector4(1, 0, 0, 1));

	// Froggo
	if (frogActive) {
	    renderer->DrawCircle(testFroggo->GetCollider()->getCentre(), 14.0f, Vector4(1, 0, 1, 1));
	}
	//Guard
	//renderer->DrawBox(testGuard->GetPosition(), Vector2(10, 16), Vector4(1, 0, 0, 1));
//	renderer->DrawCircle(testGuard->GetPosition() + testGuard->getOffset(), 16.0f, Vector4(1, 0, 1, 1));
	// Fruit 
	//renderer->DrawCircle(Vector2(250, 150), 4.0f, Vector4(1, 0, 1, 1));
	// Pixie Dust
	//renderer->DrawCircle(Vector2(285, 220), 4.0f, Vector4(1, 0, 1, 1));
	// Pixie
	//renderer->DrawBox(Vector2(350, 96), Vector2(8, 9), Vector4(1, 0, 1, 1));
	// Walls
//	renderer->DrawBox(Vector2(8, 160), Vector2(8, 160), Vector4(1, 0, 1, 1));
	//renderer->DrawBox(Vector2(472, 160), Vector2(8, 160), Vector4(1, 0, 1, 1));


	renderer->Render();

	if (player->pixieMode == true) {
		player->pixieMode = false;
		dustCount = 0;
		for (int i = 0; i <= 7; i++) {
			int x = rand() % 100;
			int y = rand() % 100;
			Pixie* newPixie = new Pixie();
			newPixie->SetPosition(Vector2(x, y));
			AddNewObject(newPixie);
			allPixies.push_back(newPixie);
		}
	}

	if (fruitCount == 0 && !frogActive) {
		frogActive = true;
		fruitCount = 0;
		SpawnFruit();
		testFroggo = new Froggo();
		testFroggo->SetPosition(Vector2(340, 280)); //340 280
		AddNewObject(testFroggo);
		addScore(5000);
	}

	if (frogActive) {
		DrawPath(testFroggo);
	}

	if (dustTime >= 30 && dustCount < 4 && (rand() % 100 < 50)) {
		dustTime = 0;
		SpawnPixieDust();
		
	}
	if (lives <= 0) {
		resetGame();
	}

}

void FruitWizardGame::SpawnPixieDust() {
	int x = rand() % currentMap->GetMapWidth();
	int y = rand() % currentMap->GetMapHeight();
	PixieDust* newDust = new PixieDust();
	newDust->SetPosition(Vector2(340, 280));
	//newDust->SetPosition(player->GetPosition());
	AddNewObject(newDust);
	allDust.push_back(newDust);
}

void FruitWizardGame::SpawnGuard() {
	std::random_device rd;
	std::uniform_int_distribution<int> distribution(1, 100);
	std::mt19937 engine(rd());
	for (int y = 0; y < currentMap->GetMapHeight(); ++y) {
		for (int x = 1; x <= currentMap->GetMapWidth() - 1; ++x) {
			MapNode node = currentMap->nodeData[(y * currentMap->GetMapWidth()) + x];
			if (node.obstacle || node.localGoal == 2) {
				continue;
			}
			int value = distribution(engine);
			if (value > 99) {
				Guard* newGuard = new Guard();
				newGuard->SetPosition(Vector2(x * 16 + 16, y * 16 + 16));
				AddNewObject(newGuard);
				break;
			}
		}
	}
}

// Draw the bounding boxes for the game's objects
void FruitWizardGame::DrawColliders(SimObject* object) {
	if (object->getType() == "obstacle" || object->getType() == "ladder") {
		CollisionVolume* collider = object->GetCollider();
		Vector2 centre = collider->getCentre();
		renderer->DrawBox(collider->getCentre(), Vector2(collider->max.x - centre.x, collider->max.y - centre.y), Vector4(1, 0, 1, 1)); // 1011
	}
	else if (object->getType() != "pixie") {
		CollisionVolume* collider = object->GetCollider();
		Vector2 centre = collider->getCentre();
		renderer->DrawCircle(collider->getCentre(), collider->max.x - collider->getCentre().x, Vector4(1, 0, 1, 1)); // 1011
	}
}

void FruitWizardGame::DrawPath(Froggo* frog) {
	for(MapNode * node : frog->getCurrentPath()) {
		renderer->DrawLine(Vector2((node->posX + 1) * 16, (node->posY + 1) * 16), Vector2((node->bestParent->posX + 1) * 16, (node->bestParent->posY + 1) * 16), Vector4(1, 1, 0, 1));
	}
	
}

void FruitWizardGame::SpawnFruit() {
	for (int i = 0; i <= 15; i++) {
		int x = rand() % currentMap->GetMapWidth();
		int y = rand() % currentMap->GetMapHeight();
		Fruit* newFruit = new Fruit();
		newFruit->SetPosition(Vector2(x * 16 + 16, y * 16 + 16));
		AddNewObject(newFruit);
	}
}

void FruitWizardGame::resetGame() {
	for (auto i = gameObjects.begin(); i != gameObjects.end();) {
		if (!((*i) == player && !((*i)->type == SimObject::OBSTACLE))) {
			(*i)->setStatus(false);
		}
		++i;
	}
	allPixies.clear();
	guards.clear();
	//allDust.clear();
	currentScore = 0;
	float gameTime = 0;
	float dustTime = 0;
	int currentScore = 0;
	int magicCount = 4;
	int dustCount = 4;
	int lives = 3;

	frogActive = false;
	player = new PlayerCharacter();
	player->SetPosition(Vector2(100, 32));
	AddNewObject(player);

	testGuard = new Guard();
	testGuard->SetPosition(Vector2(150, 224));
	testGuard->setPatrolPoint(testGuard->GetPosition());
	AddNewObject(testGuard);
	guards.push_back(testGuard);

	Guard* newGuard = new Guard();
	newGuard->SetPosition(Vector2(280, 280));
	newGuard->setPatrolPoint(newGuard->GetPosition());
	AddNewObject(newGuard);
	guards.push_back(newGuard);

	Guard* newGuard2 = new Guard();
	newGuard2->SetPosition(Vector2(310, 160));
	newGuard2->setPatrolPoint(newGuard2->GetPosition());
	AddNewObject(newGuard2);
	guards.push_back(newGuard2);

	Guard* newGuard3 = new Guard();
	newGuard3->SetPosition(Vector2(320, 88));
	newGuard3->setPatrolPoint(newGuard3->GetPosition());
	AddNewObject(newGuard3);
	guards.push_back(newGuard3);
}

void FruitWizardGame::playerStart() {
	player->SetVelocity({ 0,0 });
	player->SetPosition(Vector2(100, 32));
	frogActive = false;
	player->pixieMode = false;
	
	for (auto i = gameObjects.begin(); i != gameObjects.end();) {
		if (!((*i) == player) && !((*i)->getType() == "obstacle") && !((*i)->getType() == "fruit" && !((*i)->getType() == "guard"))) {
			(*i)->setStatus(false);
		}
		++i;
	}
	
}

void FruitWizardGame::InitialiseGame() {
	delete currentMap;
	for (auto o : gameObjects) {
		delete o;
	}
	gameObjects.clear();

	currentMap = new GameMap("FruitWizardMap.txt", gameObjects, *texManager, this);

	renderer->SetScreenProperties(16, currentMap->GetMapWidth(), currentMap->GetMapHeight());

	// Add the player and some test objects to the scene
	frogActive = false;
	player = new PlayerCharacter();
	player->SetPosition(Vector2(100, 32));
	AddNewObject(player);

	testGuard = new Guard();
	testGuard->SetPosition(Vector2(150, 224));
	testGuard->setPatrolPoint(testGuard->GetPosition());
	AddNewObject(testGuard);
	guards.push_back(testGuard);

	Guard* newGuard = new Guard();
	newGuard->SetPosition(Vector2(280, 280));
	newGuard->setPatrolPoint(newGuard->GetPosition());
	AddNewObject(newGuard);
	guards.push_back(newGuard);

	Guard* newGuard2 = new Guard();
	newGuard2->SetPosition(Vector2(310, 160));
	newGuard2->setPatrolPoint(newGuard2->GetPosition());
	AddNewObject(newGuard2);
	guards.push_back(newGuard2);
	
	Guard* newGuard3 = new Guard();
	newGuard3->SetPosition(Vector2(320, 88));
	newGuard3->setPatrolPoint(newGuard3->GetPosition());
	AddNewObject(newGuard3);
	guards.push_back(newGuard3);



	SpawnFruit();
	/*for (int i = 0; i < 3; i++) {
		SpawnGuard();
	}*/
	/*Spell* testSpell = new Spell(Vector2(0,0));
	testSpell->SetPosition(Vector2(160, 48));
	AddNewObject(testSpell);*/

	/*Fruit* testFruit = new Fruit();
	testFruit->SetPosition(Vector2(250, 150));
	AddNewObject(testFruit);

	PixieDust* testDust = new PixieDust();
	testDust->SetPosition(Vector2(285, 220));
	AddNewObject(testDust);*/

	/*Pixie* pixie = new Pixie();
	Pixie* pixie2 = new Pixie();
	pixie2->SetPosition(Vector2(350, 96));
	pixie->SetPosition(Vector2(90, 45));
	pixie->attachToPlayer();
	AddNewObject(pixie);
	AddNewObject(pixie2);*/

	/*testFroggo = new Froggo();
	testFroggo->SetPosition(Vector2(340, 285));
	AddNewObject(testFroggo);*/
	

	gameTime		= 0;
	currentScore	= 0;
	magicCount		= 4;
	dustCount		= 0;
	lives			= 3;
}

void FruitWizardGame::AddNewObject(SimObject* object) {
	newObjects.emplace_back(object);
	physics->AddRigidBody(object);
	if (object->GetCollider()) {
		physics->AddCollider(object->GetCollider());
	}
}

PlayerCharacter* FruitWizardGame::getPlayer() const {
	return player;
}

void FruitWizardGame::addScore(int score) {
	currentScore += score;
}

void FruitWizardGame::addDustCount() {
	dustCount += 1;
}

int FruitWizardGame::getDustCount() {
	return dustCount;
}

void FruitWizardGame::setLives(int count) {
	lives = count;
}
int FruitWizardGame::getLives() {
	return lives;
}

GameMap* FruitWizardGame::getCurrentMap() {
	return currentMap;
}

vector<Pixie*> FruitWizardGame::getPixies() {
	return allPixies;
}


vector<Guard*> FruitWizardGame::getGuards() {
	return guards;
}

vector<PixieDust*> FruitWizardGame::getPixieDust() {
	return allDust;
}


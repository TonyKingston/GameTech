#pragma once
#include <vector>

/*
	Modified by Tony Kingston
	File: FruitWizardGame.h
	Last Updated: 22/5/2021

*/

namespace NCL::CSC3222 {
	class GameMap;
	class SimObject;
	class GameSimsRenderer;
	class GameSimsPhysics;
	class TextureBase;
	class TextureManager;
	class PlayerCharacter;
	class Froggo;
	class Guard;
	class Pixie;
	class PixieDust;

	class FruitWizardGame {
	public:
		FruitWizardGame();
		~FruitWizardGame();

		void Update(float dt);

		void AddNewObject(SimObject* object);
		void addScore(int score);
		void addDustCount();
		int  getDustCount();
		int  getLives();
		void setLives(int lives);
		void incrementFruitCount() {
			fruitCount += 1;
		}

		void playerStart();
		GameMap* getCurrentMap();
		std::vector<Guard*> getGuards();
		std::vector<Pixie*> getPixies();
		std::vector<PixieDust*> getPixieDust();

		PlayerCharacter* getPlayer() const;

	protected:
		void InitialiseGame();
		void SpawnFruit();
		void SpawnPixieDust();
		void SpawnGuard();
		void resetGame();
		void DrawColliders(SimObject* object);
		void DrawPath(Froggo* frog);

		TextureManager*		texManager;
		GameSimsRenderer*	renderer;
		GameSimsPhysics*	physics;
		GameMap* currentMap;

		PlayerCharacter* player;
		Froggo* testFroggo;
		Guard* testGuard;

		float gameTime;
		float dustTime;

		int currentScore;
		int magicCount;
		int dustCount;
		int fruitCount;
		int lives;
		bool frogActive;

		std::vector<SimObject*> gameObjects;
		std::vector<SimObject*> newObjects;
		std::vector<Guard*> guards;
		std::vector<Pixie*> allPixies;
		std::vector<PixieDust*> allDust;
	};
}
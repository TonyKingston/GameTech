#pragma once
#include "../../Common/Vector2.h"
#include "../../Common/Vector4.h"
#include "FruitWizardGame.h"
#include "RigidBody.h"

/*
	Modified by Tony Kingston
	File: SimObject.h
	Last Updated: 22/5/2021

*/

namespace NCL {
	using namespace Maths;

	namespace Rendering {
		class TextureBase;
	}
	namespace CSC3222 {
		class GameSimsRenderer;
		class TextureManager;
		class FruitWizardGame;
		class RigidBody;
		class CollisionVolume;

		class SimObject	: public RigidBody {
		public:

			// Object types
			enum ObjType { FROGGO, FRUIT, GUARD, PIXIE, DUST, PLAYER, SPELL, OBSTACLE, LADDER, NONE};
			const char* objTypes[10] = { "froggo", "fruit", "guard", "pixie", "dust", "player", "spell", "obstacle", "ladder", "none"};

			SimObject();
			~SimObject();

			ObjType type = NONE;

			void SetCollider(CollisionVolume* c);

			CollisionVolume* GetCollider() const;

			void	UpdateAnimFrame(float dt);

			virtual bool UpdateObject(float dt) = 0;

			virtual void DrawObject(GameSimsRenderer &r);

			static void InitObjects(FruitWizardGame* game, TextureManager* manager);

			Vector2 getOffset();

			const char* getType();

			void setStatus(bool alive);

			virtual bool OnCollision(SimObject* x);

		protected:

			CollisionVolume*		collider;
			Rendering::TextureBase* texture;
			Vector2                   offset;


			Vector4			animFrameData;
			int				currentanimFrame;
			bool			flipAnimFrame;
			int				animFrameCount;
			float			frameTime;
			float			frameRate;

			bool            isAlive = true;

			static TextureManager*  texManager;
			static FruitWizardGame*		game;
		};
	}
}


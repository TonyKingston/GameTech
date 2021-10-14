#pragma once
#include <string>
#include <vector>

#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/MeshGeometry.h"
#include "FruitWizardGame.h"

/*
	Modified by Tony Kingston
	File: GameMap.h
	Last Updated: 22/5/2021

*/

namespace NCL {
	namespace Rendering {
		class TextureBase;
	}
	using namespace Maths;
	namespace CSC3222 {
		class GameSimsRenderer;
		class SimObject;
		class TextureManager;

		struct MapNode {
			MapNode* bestParent;
			int		 posX;
			int      posY;
			float    globalGoal;
			float    localGoal;
			bool     visited;
			bool     obstacle;
			vector<MapNode*> neighbours;
		};

		class GameMap	{
		public:
			GameMap(const std::string& filename, std::vector<SimObject*>& objects, TextureManager& texManager, FruitWizardGame* currentGame);
			~GameMap();

			void DrawMap(GameSimsRenderer & r);

			int GetMapWidth() const {
				return mapWidth;
			}

			int GetMapHeight() const {
				return mapHeight;
			}

			char* getMapData() const {
				return mapData;
			}

			vector<MapNode*> aStar(MapNode* start, MapNode* dest);

			Vector2 WorldPosToTilePos(const Vector2& worldPos) const;
			Vector2 TilePosToWorldPos(const Vector2& tilePos) const;

			MapNode* nodeData;
		protected:
			void BuildMapMesh();
			void AddNewTilePos(int x, int y, std::vector<Vector3>& pos, Vector2 tileSize = Vector2(16, 16));
			void AddNewTileTex(int x, int y, std::vector<Vector2>& tex, Vector2 texSize = Vector2(16,16));
			
			bool GeneratePath(const Vector2& worldFrom, const Vector2& worldTo,
				std::vector<Vector2>& path) const;

			inline double heuristic(Vector2 a, Vector2 b) {
				return std::abs(a.x - b.x) + std::abs(a.y - b.y);
			}

			inline double heuristic(MapNode* a, MapNode* b) {
				return std::abs(a->posX - b->posX) + std::abs(a->posY - b->posY);
			}

			inline double distance(MapNode* a, MapNode* b)
			{
				return sqrtf((a->posX - b->posX) * (a->posX - b->posX) + (a->posY - b->posY) * (a->posY - b->posY));
			};

			int mapWidth;
			int mapHeight;
			int structureCount;

			Rendering::TextureBase*	tileTexture;
			char*	mapData;
			int*			mapCosts;
			Vector2*		mapTexCoords;

			MeshGeometry* mapMesh;
			FruitWizardGame* currentGame;
			std::vector<SimObject*> gameObjects;
		};
	}
}


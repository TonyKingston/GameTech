#include "GameMap.h"
#include "GameSimsRenderer.h"
#include "TextureManager.h"
#include "../../Common/Maths.h"
#include "../../Common/Assets.h"
#include "../../Common/TextureLoader.h"
#include <fstream>
#include <iostream>
#include "StaticObject.h"
#include "Spell.h"
#include "AABB.h"

/*
	Modified by Tony Kingston
	File: GameMap.cpp
	Last Updated: 22/5/2021

*/


using namespace NCL;
using namespace CSC3222;
using namespace Rendering;

GameMap::GameMap(const std::string& filename, std::vector<SimObject*>& objects, TextureManager& texManager, FruitWizardGame* game)	{
	tileTexture = texManager.GetTexture("FruitWizard//arcade_platformerV2.png");
	currentGame = game;

	std::ifstream mapFile(Assets::DATADIR + filename);
	std::vector<SimObject*> gameObjects = objects;

	if (!mapFile) {
		std::cout << "GameMap can't be loaded in!" << std::endl;
		return;
	}

	mapFile >> mapWidth;
	mapFile >> mapHeight;

	mapData		= new char[mapWidth * mapHeight];
	mapCosts	= new int[mapWidth * mapHeight];
	nodeData = new MapNode[mapWidth * mapHeight];

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = ((mapHeight - 1 - y) * mapWidth) + x;
			mapFile >> mapData[tileIndex];
		}
	}

    for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = y * mapWidth + x;
			nodeData[tileIndex].posX = x;
			nodeData[tileIndex].posY = y;
			nodeData[tileIndex].obstacle = false;
			nodeData[tileIndex].bestParent = nullptr;
			nodeData[tileIndex].visited = false;
		}
	}

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			if (y > 0)
				nodeData[y * mapWidth + x].neighbours.push_back(&nodeData[(y - 1) * mapWidth + (x + 0)]);
			if (y < mapHeight - 1)
				nodeData[y * mapWidth + x].neighbours.push_back(&nodeData[(y + 1) * mapWidth + (x + 0)]);
			if (x > 0)
				nodeData[y * mapWidth + x].neighbours.push_back(&nodeData[(y + 0) * mapWidth + (x - 1)]);
			if (x < mapWidth - 1)
				nodeData[y * mapWidth + x].neighbours.push_back(&nodeData[(y + 0) * mapWidth + (x + 1)]);
		}
	}

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = ((mapHeight - 1 - y) * mapWidth) + x;
			char c;
			mapFile >> c;
			mapCosts[tileIndex] = c - '0';
			if (c == 'o' || c == '0') {
				nodeData[tileIndex].obstacle = true;
			}
		}
	}
	
	BuildMapMesh();

}

/// <summary>
/// Implementation of the A* pathfinding algorithm
/// </summary>
/// <param name="start">The starting node</param>
/// <param name="dest">The destination node to find a path</param>
/// <returns>Returns a vector of nodes representing the path, if possible, from start to dest. The vector starts with dest and ends with start</returns>
vector<MapNode*> GameMap::aStar(MapNode* start, MapNode* dest) {
	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = (y * mapWidth) + x;
			nodeData[tileIndex].visited = false;
			nodeData[tileIndex].globalGoal = INFINITY;
			nodeData[tileIndex].localGoal = INFINITY;
			nodeData[tileIndex].bestParent = nullptr;
		}
	}

	MapNode* currentNode = start;
	currentNode->localGoal = 0.0f;
	currentNode->globalGoal = heuristic(start, dest);

	std::list<MapNode*> openList;
	openList.push_back(start);

	while (!openList.empty()) {

		openList.sort([](const MapNode* lhs, const MapNode* rhs) {
			return lhs->globalGoal < rhs->globalGoal; });

		while (!openList.empty() && openList.front()->visited) {
			openList.pop_front();
		}

		if (openList.empty())
			break;

		currentNode = openList.front();
		currentNode->visited = true;

		for (auto neighbour : currentNode->neighbours) {
			if (!neighbour->visited && !neighbour->obstacle) {
				openList.push_back(neighbour);
			}

			float newGoal = currentNode->localGoal + (mapCosts[(neighbour->posY * mapWidth) + neighbour->posX]);
		    
			if (newGoal < neighbour->localGoal) {
				neighbour->bestParent = currentNode;
				neighbour->localGoal = newGoal;

				neighbour->globalGoal = neighbour->localGoal + heuristic(neighbour, dest);
			}
		
		}
	}

	vector<MapNode*> path;
	MapNode* node = dest;

	// Get list of nodes to traverse
	while (node->bestParent != NULL) {
		path.push_back(node);
		node = node->bestParent;
		
	}
	if (node->bestParent == NULL) {
		node->bestParent == node;
	}

	return path;
}

GameMap::~GameMap()	{
	delete[] mapData;
	delete[] mapCosts;
	delete[] mapTexCoords;
	delete[] nodeData;

	delete mapMesh;
}

void GameMap::DrawMap(GameSimsRenderer & r) {
	r.DrawMesh((OGLMesh*)mapMesh, (OGLTexture*)tileTexture);
}

void GameMap::BuildMapMesh() {
	vector<Vector2> texCoords;
	vector<Vector3> positions;

	Vector2 primaryTile;
	StaticObject* wall = NULL;
	StaticObject* ground = NULL;
	int tileCount = 0;
	int lastPos = 0;

	AddNewTilePos(0, 0, positions, Vector2(mapWidth *16.0f, mapHeight * 16.0f));
	AddNewTileTex(0, 176, texCoords, Vector2(160, 128));

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileType = mapData[(y * mapWidth) + x];

			switch (tileType) {
			case '0': {
				continue;
			}break; //Top of ladder
				//the normal tile types first
				case 'T':primaryTile = Vector2(320, 0); break; //Top of ladder
				case 'L':primaryTile = Vector2(320, 16); break; //Ladder
				case 'B':primaryTile = Vector2(320, 32); break; //Bottom of ladder

				case 't':primaryTile = Vector2(336, 0); break; //Top of ladder
				case 'l':primaryTile = Vector2(336, 16); break; //Ladder
				case 'b':primaryTile = Vector2(336, 32); break; //Bottom of ladder

				case 'G': {
					primaryTile = Vector2(224, 48);
				}break;//Random grass tiles
				case 'F': { //Left End
					primaryTile = Vector2(208, 48);
				}break;//Random grass tiles
				case 'H': { //Right End
					primaryTile = Vector2(240, 48);
				}break;//Random grass tiles

				case 'W': {//wall
					primaryTile = Vector2(256, 112);
				}break;//Random grass tiles
				case 'w': {//wall top
					// Setup a bounding volume for the whole wall instead of each wall tile
					primaryTile = Vector2(256, 96);
					wall = new StaticObject(false);
					wall->SetPosition(Vector2(x * 16 + 8, (mapHeight / 2) * 16));
					CollisionVolume* collider = new AABB(wall->GetPosition(), 8, (mapHeight / 2)*16);
					wall->SetCollider(collider);
					currentGame->AddNewObject(wall);
				}break;//Random grass tiles
			}			
			AddNewTilePos(x, y, positions);
			AddNewTileTex((int)primaryTile.x, (int)primaryTile.y, texCoords);
		}
		
	}
	mapMesh = new OGLMesh();
	mapMesh->SetVertexPositions(positions);
	mapMesh->SetVertexTextureCoords(texCoords);
	mapMesh->SetPrimitiveType(NCL::GeometryPrimitive::Triangles);
	mapMesh->UploadToGPU();

	// Create a bounding for each platform instead of individual tiles
	int offset = 0;
	for (int y = 0; y < mapHeight; ++y) {
		tileCount = 0;
		for (int x = 1; x <= mapWidth - 1; ++x) {
			int ladderCount = 0;
			int tileType = mapData[(y * mapWidth) + x];
			if (tileType == '0' || tileType == 'L' || tileType == 'l' || tileType == 'B' || tileType == 'b') {
				continue;
			}
			// Create bounding boxes for the ladder tiles
			if (tileType == 'T' ) {
				ladderCount += 1;
				while (mapData[((y - ladderCount) * mapWidth) + x] != 'B') {
					ladderCount += 1;
				}

				ground = new StaticObject(true);
				int midPoint = ((lastPos - ladderCount + 1) + lastPos) / 2;
				ground->SetPosition(Vector2(x * 16 + 16, (y - ladderCount / 2) * 16));
				CollisionVolume* collider = new AABB(ground->GetPosition(), 16, ((ladderCount * 16) + 16)/ 2);
				ground->SetCollider(collider);
				currentGame->AddNewObject(ground);

			}
			if (tileType == 'H' || x == mapWidth - 1 || tileType == 'T' || tileType == 't') {
				if (tileType == 'H') {
					tileCount += 1;
					lastPos = x;
				}
				if (tileCount > 0) {
					if (tileCount % 2 == 0) {
						offset = 16;
					}
					else {
						offset = 8;
					}
					ground = new StaticObject(false);
					int midPoint = ((lastPos - tileCount + 1) + lastPos) / 2;
					ground->SetPosition(Vector2((midPoint * 16) + offset, (y * 16) + 8));
					CollisionVolume* collider = new AABB(ground->GetPosition(), (tileCount * 16) / 2, 8);
					ground->SetCollider(collider);
					currentGame->AddNewObject(ground);
				}
				tileCount = 0;
			}
			else {
				tileCount += 1;
				lastPos = x;
			}
		}

	}
}

void GameMap::AddNewTilePos(int x, int y, std::vector<Vector3>& pos, Vector2 tileSize) {
	Vector3 topLeft		= Vector3((x + 0) * tileSize.x, (y + 0) * tileSize.y, 0);
	Vector3 topRight	= Vector3((x + 1) * tileSize.x, (y + 0) * tileSize.y, 0);
	Vector3 bottomLeft	= Vector3((x + 0) * tileSize.x, (y + 1) * tileSize.y, 0);
	Vector3 bottomRight = Vector3((x + 1) * tileSize.x, (y + 1) * tileSize.y, 0);

	pos.emplace_back(topLeft);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(topRight);

	pos.emplace_back(topRight);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(bottomRight);
}

void GameMap::AddNewTileTex(int x, int y, std::vector<Vector2>& tex, Vector2 texSize) {
	Vector2 topLeft		= Vector2((x + 0.0f) , (y + texSize.y));
	Vector2 topRight	= Vector2((x + texSize.x), (y + texSize.y));
	Vector2 bottomLeft	= Vector2((x + 0.0f) , (y + 0.0f));
	Vector2 bottomRight = Vector2((x + texSize.x), (y + 0.0f));

	tex.emplace_back(topLeft);
	tex.emplace_back(bottomLeft); 
	tex.emplace_back(topRight);

	tex.emplace_back(topRight);
	tex.emplace_back(bottomLeft);
	tex.emplace_back(bottomRight);
}

Vector2 GameMap::WorldPosToTilePos(const Vector2& worldPos) const {
	return Vector2(trunc((worldPos.x / 16) + 0.5) - 1, trunc((worldPos.y / 16) + 0.5) - 1);
}
Vector2 GameMap::TilePosToWorldPos(const Vector2& tilePos) const {
	return Vector2(trunc(tilePos.x + 1 * 16), trunc(tilePos.y + 1 * 16));
}
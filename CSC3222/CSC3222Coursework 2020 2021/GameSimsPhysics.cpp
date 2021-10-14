#include "GameSimsPhysics.h"
#include "RigidBody.h"
#include "CollisionVolume.h"
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Maths.h"
#include "SimObject.h"
#include <iostream>
#include <random>
#include <typeinfo>
#include <typeindex>
#include "AABB.h"
#include "Circle.h"

/*
	Modified by Tony Kingston
	File: GameSimsPhysics.cpp
	Last Updated: 22/5/2021

*/

using namespace NCL;
using namespace CSC3222;

GameSimsPhysics::GameSimsPhysics()	{

}


GameSimsPhysics::~GameSimsPhysics()	{

}

void GameSimsPhysics::Update(float dt) {
	cPair.normal = Vector2(0,0);
	cPair.penetration = 0.0f;
	Integration(dt);
	CollisionDetection(dt);
}

/// <summary>
/// Adds a rigid body to the list of all bodies
/// </summary>
/// <param name="b">Pointer to RigidBody to be added to list</param>
void GameSimsPhysics::AddRigidBody(RigidBody* b) {
	allBodies.emplace_back(b);
	if (!b->isStatic()) {
		allDynamicBodies.emplace_back(b);
	}
}

/// <summary>
/// Removes a RigidBody from the list of all bodies
/// </summary>
/// <param name="b">Pointer to RigidBody to be removed</param>
void GameSimsPhysics::RemoveRigidBody(RigidBody* b) {
	auto at = std::find(allBodies.begin(), allBodies.end(), b);

	if (at != allBodies.end()) {
		//maybe delete too?
		allBodies.erase(at);
	}
	at = std::find(allDynamicBodies.begin(), allDynamicBodies.end(), b);
	if (at != allDynamicBodies.end()) {
		//maybe delete too?
		allDynamicBodies.erase(at);
	}
}

/// <summary>
/// Adds a collider to the list of all colliders
/// If the collider's object is static, the collider is added to a list of static collider else it is added to a list of 
/// dynamic colliders.
/// The class resorts the list of static colliders every time a new one is added. This is more efficient than unnecessarily sort every frame as the positions
/// of the static objects won't change.
/// </summary>
/// <param name="c">Pointer to CollisionVolume to add to list</param>
void GameSimsPhysics::AddCollider(CollisionVolume* c) {
	allColliders.emplace_back(c);
	if (c->getObject()->isStatic()) {
		allStaticColliders.emplace_back(c);
		// No need to sort static objects every frame 
        sort(allStaticColliders.begin(), allStaticColliders.end(), compare);
	}
	else {
		allDynamicColliders.emplace_back(c);
	}
}

/// <summary>
/// Removes a collider from the list of all colliders
/// </summary>
/// <param name="c"></param>
void GameSimsPhysics::RemoveCollider(CollisionVolume* c) {
	auto at = std::find(allColliders.begin(), allColliders.end(), c);

	if (at != allColliders.end()) {
		//maybe delete too?
		allColliders.erase(at);
	}

	at = std::find(allDynamicColliders.begin(), allDynamicColliders.end(), c);
	if (at != allDynamicColliders.end()) {
		//maybe delete too?
		allDynamicColliders.erase(at);
	}

}

/// <summary>
/// Performs Euler intergration on the acceleration and velocity of all dynamic bodies, updating their position and resetting the applied force to zero
/// </summary>
/// <param name="dt">Time step</param>
void GameSimsPhysics::Integration(float dt) {
	for (RigidBody* body : allDynamicBodies) {
	
		// Integrate acceleration
		Vector2 a = body->force * body->inverseMass;
		body->SetVelocity(body->GetVelocity() + (a * dt));

		// Clamp the max velocity of the object
		if (body->velocity.Length() > body->maxVelocity) {
			body->SetVelocity(body->GetVelocity().Normalised() * body->maxVelocity);
		}

		// Integrate velocity
		body->SetVelocity(body->GetVelocity() * body->damping); // Damping factor
		body->SetPosition(body->GetPosition() + (body->GetVelocity() * dt));
		

		body->force = Vector2(0, 0);


		for (int i = 0; i < allDynamicColliders.size(); i++) {
			allDynamicColliders[i]->updatePosition();
		}

		/*for (int i = 0; i < allColliders.size(); i++) {
			allColliders[i]->updatePosition();
		}*/
	}
}

/// <summary>
/// Determines if two collision volumes overlap
/// </summary>
/// <param name="shapeA">Pointer to CollisionVolume</param>
/// <param name="shapeB">Pointer to CollisionVolume</param>
/// <returns>True if they overlap else false</returns> 
bool AABBOverlap(CollisionVolume* a, CollisionVolume* b) {
	if (a->max[0] < b->min[0] || a->min[0] > b->max[0]) return 0;
	if (a->max[1] < b->min[1] || a->min[1] > b->max[1]) return 0;
	return true;
}

/// <summary>
/// Compares two collision volumes for the purpose of sorting. Gets the minimum coordinates, on the current sorting axis, of the volumes and compares them. The volume with the lower
/// value should come first when sorting.
/// </summary>
/// <param name="a">Shape: LHS comparision</param>
/// <param name="b">Shape: RHS comparision</param>
/// <returns>True if minA < minB</returns>
int GameSimsPhysics::compare(CollisionVolume* a, CollisionVolume* b)
{
	float minA = a->min[sortAxis];
	float minB = b->min[sortAxis];
	return minA < minB;
}


/// <summary>
/// Uses sort and sweep for broad phase collision detection and then checks if the relevant pairs of collision volumes collide.
/// This method also calculates the best axis to perform sort and sweep on i.e. the one with the most variance of the collision volume centres.
/// This will improve performance assuming objects aren't clustered on both axes
/// </summary>
/// <param name="dt">Time step</param>
void GameSimsPhysics::CollisionDetection(float dt) {
	// I've provided two methods of collision detection. One just using the list of all colliders and one that iterates the static and dynamic lists
	// to check collision between static/dynamic and dynamic/dynamic volumes. Both work although I've yet to determine the performance difference between
	// iterating over the one list and skipping the collison detection if both objects are static or using the methods which maintains two lists.

	// Sort list of colliders using the compare function
	sort(allColliders.begin(), allColliders.end(), compare);
	// Set up array to compute variance of volume centres
	float s[2] = { 0.0f, 0.0f }, s2[2] = { 0.0f, 0.0f }, v[2];
	
	for (int i = 0; i < allColliders.size(); i++) {
		Vector2 centre = allColliders[i]->getCentre();
		bool isStatic = allColliders[i]->getObject()->isStatic();

		for (int c = 0; c < 2; c++) {
			s[c] += centre[c];
			s2[c] += centre[c] * centre[c];
		}
		
		// For each collision volume, iterate through the rest of the volumes
		for (int j = i + 1; j < allColliders.size(); j++) {
	
			// Stop if the next volume is beyon the max coordinate of the current volume
			if (allColliders[j]->min[sortAxis] > allColliders[i]->max[sortAxis]) {
				break;
			}
			// Jump to the next volume if both are static
			if (isStatic && allColliders[j]->getObject()->isStatic()) {
				continue;
			}
			//std::cout << allColliders[i]->min << " " << allColliders[i]->max << " " << allColliders[i]->getCentre() << std::endl;
			
			// If the AABBs overlap then there is a chance of collision
			if (AABBOverlap(allColliders[i], allColliders[j])) {
			//	if (allColliders[i]->getObject()->getType() == "player" || allColliders[i]->getObject()->getType() == "obstacle") {
				//	
			//		std::cout << "Possible Collision Detected between " << allColliders[i]->getObject()->getType() << " and " <<
			//			allColliders[j]->getObject()->getType() << std::endl;
			//	}
				// Check if the collision volumes do in fact collide
			//	if (allColliders[i]->Collides(*allColliders[j])) {
					//if (allColliders[i]->getObject()->getType() == "spell" || (allColliders[j]->getObject()->getType() == "spell")) {
					//	std::cout << "Collision Detected between " << allColliders[i]->getObject()->getType() << " and " <<
						//	allColliders[j]->getObject()->getType() << std::endl;
				// Only resolve collision if it is needed
						if (allColliders[i]->getObject()->OnCollision(allColliders[j]->getObject())) {
							Collides(allColliders[i], allColliders[j]);
							ProjectionResolution(allColliders[i], allColliders[j]);
							ImpulseResolution(allColliders[i], allColliders[j]);
						}
				//	}
					// Resolve Collision:

			//	}
			}
		}

		for (int c = 0; c < 2; c++) {
			v[c] = s2[c] - s[c] / allColliders.size();
		}
		sortAxis = 0;
		// Update axis to perform sort and sweep on to the one with the greatest variance, potential performance gain
	//	if (v[1] > v[0]) sortAxis = 1;
	}

	/*sort(allDynamicColliders.begin(), allDynamicColliders.end(), compare);
	float s[2] = { 0.0f, 0.0f }, s2[2] = { 0.0f, 0.0f }, v[2];

	for (int i = 0; i < allStaticColliders.size(); i++) {
		Vector2 centre = allStaticColliders[i]->getCentre();
		
		for (int c = 0; c < 2; c++) {
			s[c] += centre[c];
			s2[c] += centre[c] * centre[c];
		}

		for (int k = 0; k < allDynamicColliders.size(); k++) {
			if (allDynamicColliders[k]->min[sortAxis] > allStaticColliders[i]->max[sortAxis]) {
				break;
			}
			if (AABBOverlap(allStaticColliders[i], allDynamicColliders[k])) {
				//if (allDynamicColliders[i]->getObject()->getType() == "player" || allDynamicColliders[i]->getObject()->getType() == "obstacle") {

					//std::cout << "Possible Collision Detected between " << allColliders[i]->getObject()->getType() << " and " <<
					//	allColliders[k]->getObject()->getType() << std::endl;
				//}
				if (allStaticColliders[i]->Collides(*allDynamicColliders[k])) {
					std::cout << "Collision Detected between " << allStaticColliders[i]->getObject()->getType() << " and " <<
						allDynamicColliders[k]->getObject()->getType() << std::endl;
					// Resolve Collision

				}
			}
			
		}
	}
	for (int i = 0; i < allDynamicColliders.size(); i++) {
		Vector2 centre = allDynamicColliders[i]->getCentre();
		//if (allDynamicColliders[i]->getObject()->getType() == "player") {
		  //  std::cout << allDynamicColliders[i]->getObject()->GetPosition() << std::endl;
		//}

		// Calculate variance of AABB centres
		for (int c = 0; c < 2; c++) {
			s[c] += centre[c];
			s2[c] += centre[c] * centre[c];
		}

		for (int j = i + 1; j < allDynamicColliders.size(); j++) {
			if (allDynamicColliders[j]->min[sortAxis] > allDynamicColliders[i]->max[sortAxis]) {
				break;
			}
			//std::cout << allColliders[i]->min << " " << allColliders[i]->max << " " << allColliders[i]->getCentre() << std::endl;

			if (AABBOverlap(allDynamicColliders[i], allDynamicColliders[j])) {
				//if (allDynamicColliders[i]->getObject()->getType() == "player" || allDynamicColliders[i]->getObject()->getType() == "obstacle") {

					//std::cout << "Possible Collision Detected between " << allColliders[i]->getObject()->getType() << " and " <<
					//	allColliders[j]->getObject()->getType() << std::endl;
				//}
				if (allDynamicColliders[i]->Collides(*allDynamicColliders[j])) {
					//	std::cout << "Collision Detected between " << allColliders[i]->getObject()->getType() << " and " <<
						//	allColliders[j]->getObject()->getType() << std::endl;

				}
			}
		}


		for (int c = 0; c < 2; c++) {
			v[c] = s2[c] - s[c] / allColliders.size();
		}
		sortAxis = 0;
		// Update axis to be sorted to the one with the greatest AABB variance, potential performance gain
		if (v[1] > v[0]) sortAxis = 1;
	
	}*/

	
}

void GameSimsPhysics::ResolveCollision(CollisionVolume* a, CollisionVolume* b) {
	SimObject* currentObject = (SimObject*)b->getObject();
    SimObject* testObject = (SimObject*)a->getObject();
}

void GameSimsPhysics::ImpulseResolution(CollisionVolume* a, CollisionVolume* b) {
	SimObject* objA = a->getObject();
	SimObject* objB = b->getObject();
	Vector2 normal = cPair.normal;
	float totalMass = objA->GetInverseMass() + objB->GetInverseMass();
	Vector2 relativeVelocity = objB->GetVelocity() - objA->GetVelocity();
	float e = objA->GetElasticity() * objB->GetElasticity();
	float J = (-(1 + e) * Vector2::Dot(relativeVelocity, normal)) / totalMass;

	Vector2 newVelocity = objA->GetVelocity() - normal * (objA->GetInverseMass() * J);
	Vector2 newVelocity2 = objB->GetVelocity() + normal * (objB->GetInverseMass() * J);
	objA->AddImpulse(newVelocity);
	objB->AddImpulse(newVelocity2);
}

void GameSimsPhysics::ProjectionResolution(CollisionVolume* a, CollisionVolume* b) {
	SimObject* objA = a->getObject();
	SimObject* objB = b->getObject();

	float totalMass = objA->GetInverseMass() + objB->GetInverseMass();
	objA->SetPosition(objA->GetPosition() - cPair.normal * cPair.penetration * (objA->GetInverseMass() / totalMass));
	objB->SetPosition(objB->GetPosition() + cPair.normal * cPair.penetration * (objB->GetInverseMass() / totalMass));
}

bool GameSimsPhysics::Collides(CollisionVolume* a, CollisionVolume* b) {
	if (dynamic_cast<AABB*>(a)) {
	if (dynamic_cast<Circle*>(b))
		return Collides((AABB*)a, (Circle*)b);
	else if (dynamic_cast<AABB*>(b))
		return Collides((AABB*)a, (AABB*)b);
	else
		return false;
	}
	else if (dynamic_cast<Circle*>(a)) {
		if (dynamic_cast<Circle*>(b))
			return Collides((Circle*)a, (Circle*)b);
		else if (dynamic_cast<AABB*>(b))
			return Collides((Circle*)a, (AABB*)b);
		else
			return false;
	}
	else {
		return false;
	}
	
}

bool GameSimsPhysics::Collides(AABB* a, AABB* b) {
	//float overlapX = a->getCentre().x - b->getCentre().x * ((a->halfWidth + b->halfWidth) - abs(a->getCentre().x - b->getCentre().x));
	//float overlapY = a->getCentre().y - b->getCentre().y * ((a->halfHeight + b->halfHeight) - abs(a->getCentre().y - b->getCentre().y));
	float depthX = (a->halfWidth + b->halfWidth) - abs(a->getCentre().x - b->getCentre().x);
	float depthY = (a->halfHeight + b->halfHeight) - abs(a->getCentre().y - b->getCentre().y);
	if (depthX > 0 && depthY > 0) {
		Vector2 collisionNormal = Vector2(a->getCentre().x - depthX, a->getCentre().y - depthY).Normalised();
		cPair.normal = depthX < depthY ? Vector2(1, 0) : Vector2(0, 1);
		cPair.penetration = depthX < depthY ? depthX : depthY;
		return true;
	}
	return false;
}

bool GameSimsPhysics::Collides(Circle* a, Circle* b) {
	float distance = (a->getCentre() - b->getCentre()).Length();
	float sumRadi = a->getRadius() + b->getRadius();

	if (distance < sumRadi) {
		cPair.penetration = sumRadi - distance;
		cPair.normal = (a->getCentre() - b->getCentre()).Normalised();
		return true;
	}

	return false;
}

bool GameSimsPhysics::Collides(AABB* a, Circle* b) {
	if (Collides(b, a)) {
		cPair.normal = -cPair.normal;
		return true;
	}
	return false;
}

bool GameSimsPhysics::Collides(Circle* a, AABB* b) {
	Vector2 nearestPoint = Clamp(a->getCentre(), b->getCentre() - Vector2(b->halfWidth, b->halfHeight), b->getCentre() + Vector2(b->halfWidth, b->halfHeight));
	float distance = (a->getCentre() - nearestPoint).Length();

	if (distance < a->getRadius()) {
		cPair.normal = (nearestPoint - a->getCentre()).Normalised();
		cPair.penetration = a->getRadius() - distance;
		return true;
	}
	return false;
}






#include "RigidBody.h"

/*
	Modified by Tony Kingston
	File: RigidBody.cpp
	Last Updated: 22/5/2021

*/

using namespace NCL;
using namespace CSC3222;

RigidBody::RigidBody()	{
	inverseMass = 1.0f;
	elasticity  = 0.5f;
	maxVelocity = 64;
	damping = 1.0f;
	position = Vector2(0, 0);
}


RigidBody::~RigidBody()	{

}

/*
 *  PolyPhysicsScreen.h
 *  Poly
 *
 *  Created by Ivan Safrin on 5/8/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

// @package ScreenPhysics

#pragma once
#include "PolyGlobals.h"
#include "PolyScreen.h"
#include "Box2D.h"
//#include "PolyCoreServices.h"
#include "PolyScreenLine.h"
#include "PolyPhysicsScreenEntity.h"
#include "PolyTimer.h"
#include <vector>

#define MAX_B2DCONTACTPOINTS 2048

namespace Polycode {
	
class _PolyExport PhysicsScreenEvent : public Event {
	public:	
		
		PhysicsScreenEntity *entity1;	
		PhysicsScreenEntity *entity2;	
	
		static const int EVENT_NEW_SHAPE_COLLISION = 0;
		static const int EVENT_END_SHAPE_COLLISION = 1;
		static const int EVENT_PERSIST_SHAPE_COLLISION = 2;	
};		


enum ContactState
{
	e_contactAdded = 0,
	e_contactPersisted = 1,
	e_contactRemoved = 2,
};
	
struct ContactPoint
{
	b2Shape* shape1;
	b2Shape* shape2;
	b2Vec2 normal;
	b2Vec2 position;
	b2Vec2 velocity;
	b2ContactID id;
	ContactState state;
};
	
	
class _PolyExport PhysicsScreen : public Screen, b2ContactListener {

public:
	PhysicsScreen();
	PhysicsScreen(float freq);
	
	PhysicsScreen(Vector2 physicsWorldLowerBound, Vector2 physicsWorldUpperBound);
	~PhysicsScreen();
	
	void Update();
	PhysicsScreenEntity *addPhysicsChild(ScreenEntity *newEntity, int entType, float friction, float density, float restitution = 0, bool isSensor = false);
	void removePhysicsChild(PhysicsScreenEntity *entityToRemove);
	
	PhysicsScreenEntity *addCollisionChild(ScreenEntity *newEntity, int entType);
	
	void createDistanceJoint(ScreenEntity *ent1, ScreenEntity *ent2, bool collideConnected);
	void createPrismaticJoint(ScreenEntity *ent1, ScreenEntity *ent2, bool collideConnected);
	b2RevoluteJoint *createRevoluteJoint(ScreenEntity *ent1, ScreenEntity *ent2, float ax, float ay, bool enableLimit, float lowerLimit, float upperLimit, bool motorEnabled, float motorSpeed, float maxTorque);
	b2MouseJoint *createMouseJoint(ScreenEntity *ent1, Vector2 *mp);
	void applyForce(ScreenEntity *ent, float fx, float fy);
	void applyImpulse(ScreenEntity *ent, float fx, float fy);
	
	PhysicsScreenEntity *getPhysicsEntityByShape(b2Shape *shape);
	
	void setVelocity(ScreenEntity *ent, float fx, float fy);	
	void setVelocityX(ScreenEntity *ent, float fx);	
	void setVelocityY(ScreenEntity *ent, float fy);	
	
	void Add(const b2ContactPoint* point);
	void Persist(const b2ContactPoint* point);
	void Remove(const b2ContactPoint* point);				
	
	void wakeUp(ScreenEntity *ent);
	
	void handleEvent(Event *event);
	
	Vector2 getEntityCollisionNormal(ScreenEntity *ent1, ScreenEntity *ent2);
	
	bool areEntitiesColliding(ScreenEntity *ent1, ScreenEntity *ent2);
	ScreenEntity *getEntityAtPosition(float x, float y);
	bool testEntityAtPosition(ScreenEntity *ent, float x, float y);
	
	void Shutdown();
	
	PhysicsScreenEntity *getPhysicsByScreenEntity(ScreenEntity *ent);
	void destroyMouseJoint(b2MouseJoint *mJoint);


protected:
	
	void init(Vector2 physicsWorldLowerBound, Vector2 physicsWorldUpperBound, float physicsTimeStep, int physicsIterations, Vector2 physicsGravity);

	Timer *updateTimer;
	vector <PhysicsScreenEntity*> physicsChildren;
	
	ContactPoint m_points[MAX_B2DCONTACTPOINTS];
	int32 numContactPoints;
	
	b2World *world;
	float32 timeStep;
	int32 iterations;
};


}
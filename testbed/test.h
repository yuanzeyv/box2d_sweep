#ifndef TEST_H
#define TEST_H

#include "box2d/box2d.h"
#include "draw.h" 

#include <stdlib.h>
#include "Library/sol/sol.hpp" 
#include "Manager/ColliderListenerManager.h" 
struct Settings;
class Test;   
class DestructionListener : public b2DestructionListener
{
public:
	void SayGoodbye(b2Fixture* fixture) override { B2_NOT_USED(fixture); }
	void SayGoodbye(b2Joint* joint) override; 
	Test* test;
};   
class Test
{
public:

	Test();
	virtual ~Test();
	virtual void Step(Settings& settings);
	virtual void UpdateUI() {}
	virtual void Keyboard(int key) { B2_NOT_USED(key); }
	virtual void KeyboardUp(int key) { B2_NOT_USED(key); }
	virtual void KeyboardRepeat(int key) { B2_NOT_USED(key); } 
	virtual void MouseDown(const b2Vec2& p);
	virtual void MouseUp(const b2Vec2& p);
	virtual void MouseMove(const b2Vec2& p);  
	void ShiftOrigin(const b2Vec2& newOrigin);  

	virtual void DebugDraw(b2World* world, float step);
protected:
	friend class DestructionListener;
	friend class BoundaryListener;
	friend class ContactListener;

	b2Body* m_groundBody;
	b2AABB m_worldAABB;  
	int32 m_pointCount;
	DestructionListener m_destructionListener;
	int32 m_textLine;
	b2World* m_world;
	b2Body* m_bomb;
	b2MouseJoint* m_mouseJoint;
	b2Vec2 m_bombSpawnPoint;
	bool m_bombSpawning;
	b2Vec2 m_mouseWorld;
	int32 m_stepCount;
	int32 m_textIncrement;
	b2Profile m_maxProfile;
	b2Profile m_totalProfile; 
};

typedef Test* TestCreateFcn();
int RegisterTest(const char* category, const char* name, TestCreateFcn* fcn);

//
struct TestEntry
{
	const char* category;
	const char* name;
	TestCreateFcn* createFcn;
};

#define MAX_TESTS 256
extern TestEntry g_testEntries[MAX_TESTS];
extern int g_testCount;

#endif

#ifndef _PHYSICS_PARSE_H_
#define _PHYSICS_PARSE_H_  
#include <map>
#include <string> 
#include <iostream>
#include "FixtureBase.h"  
using namespace std;
class PhysicsParse {
private:
	PhysicsParse();//Ë½ÓÐ¹¹ÔìÆ÷
public:
	static void ParsePhysicsParse(string path);
	void InsertFixtureBase(FixtureBase& base);
	vector<FixtureBase>* FindFixtureVector(string name); 
private:
	map<string,vector<FixtureBase>> BodyModelMap;
public:
	static PhysicsParse& Instance();

	b2Body* CreateBody(b2Body* body, string name);
	b2Body* CreateBody(b2World* world, string name, b2Vec2 pos , b2BodyType type = b2BodyType::b2_dynamicBody);
}; 
#endif //  _PHYSICS_PARSE_H_

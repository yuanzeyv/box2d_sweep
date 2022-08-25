#pragma once
#include "LuaBridge/LuaBridge.h" 
#include "Box2d/box2d.h"
class B2FixturePort :public SOLPort {
public:
	void InitB2Filter(sol::table solTable);
	void InitB2FixtureDef(sol::table solTable);
	void InitB2FixtureProxy(sol::table solTable);
	void InitB2Fixture(sol::table solTable);
	void InitB2FixtureUserData(sol::table solTable);
	void InitTable(sol::table table) override;
};
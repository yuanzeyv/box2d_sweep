#pragma once
#include "LuaBridge/LuaBridge.h" 
#include "Box2d/box2d.h"
class PlayerManagerProxy :public SOLPort {
public:
	void InitPlayerManagerPort(sol::table solTable);
	void InitPlayerBodyData(sol::table solTable);
	void InitTable(sol::table solTable) override
	{ 
		InitPlayerManagerPort(solTable);
		InitPlayerBodyData(solTable);
	}
};
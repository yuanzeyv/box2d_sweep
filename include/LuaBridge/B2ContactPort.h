#pragma once
#include "LuaBridge/LuaBridge.h" 
#include "Box2d/box2d.h"
class B2ContactPort :public SOLPort {
public:
	void InitB2ContactEdgeTable(sol::table solTable);
	void InitB2ContactTable(sol::table solTable);
	void InitTable(sol::table solTable) override;
};
#pragma once
#include "LuaBridge/LuaBridge.h"  
class B2ShapePort :public SOLPort {
public:
	void InitB2MassData(sol::table solTable);
	void  InitB2Shape(sol::table solTable);
	void  InitB2ChainShape(sol::table solTable);
	void  InitB2CircleShape(sol::table solTable);
	void  InitB2EdgeShape(sol::table solTable);
	void  InitB2PolygonShape(sol::table solTable);
	void InitTable(sol::table solTable) override;
};
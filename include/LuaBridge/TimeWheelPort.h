#pragma once
#include "LuaBridge/LuaBridge.h" 
#include "Box2d/box2d.h"
class TimeWheelPort :public SOLPort {
public:
	void InitTimeWheelEvecntPort(sol::table solTable);
	void InitTimeWheelManagerPort(sol::table solTable);
	void InitTable(sol::table solTable) override
	{ 
		InitTimeWheelEvecntPort(solTable);
		InitTimeWheelManagerPort(solTable);
	}
};
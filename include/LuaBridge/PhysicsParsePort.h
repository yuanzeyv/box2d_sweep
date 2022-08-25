#pragma once
#include "LuaBridge/LuaBridge.h" 
class PhysicsParsePort :public SOLPort {
public: 
	void InitPhysicsParse(sol::table solTable);
	void InitFixtureBase(sol::table solTable);
	void InitShapeBaseMap(sol::table solTable);
	void InitShapeBase(sol::table solTable);
	void InitPolygonShapeBase(sol::table solTable);
	void InitCircleShapeBase(sol::table solTable);

	void InitTable(sol::table solTable) override;
};
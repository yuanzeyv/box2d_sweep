#pragma once
#include "LuaBridge/LuaBridge.h" 
class B2CollisionPort :public SOLPort {
public: 
	void InitB2ContactFeature(sol::table solTable);
	void InitB2ManifoldPoint(sol::table solTable);
	void InitB2Manifold(sol::table solTable);

	void InitB2WorldManifold(sol::table solTable);
	void InitB2ClipVertex(sol::table solTable);
	void InitB2RayCastInput(sol::table solTable);
	void InitB2RayCastOutput(sol::table solTable);
	void InitB2AABB(sol::table solTable);
	void InitTable(sol::table solTable) override;
};
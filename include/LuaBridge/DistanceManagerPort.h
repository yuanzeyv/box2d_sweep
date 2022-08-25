#pragma once
#include "LuaBridge/LuaBridge.h" 
class DistanceManagerPort :public SOLPort {
public: 
	void InitDistanceManager(sol::table solTable);
	void InitVisibleMap(sol::table solTable);
	void InitTable(sol::table table) override;
};
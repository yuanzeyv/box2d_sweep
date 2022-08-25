#pragma once
#include "LuaBridge/LuaBridge.h" 
class B2BroadPhasePort :public SOLPort {
public:
	void InitB2Pair(sol::table solTable);
	void  InitB2BroadPhase(sol::table solTable);
	void InitTable(sol::table table) override;
};
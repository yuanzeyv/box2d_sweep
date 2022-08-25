#pragma once
#include "LuaBridge/LuaBridge.h"  
class BodyManagerPort :public SOLPort {
public: 
	void InitBodyManager(sol::table solTable);
	void InitTable(sol::table table) override;
};
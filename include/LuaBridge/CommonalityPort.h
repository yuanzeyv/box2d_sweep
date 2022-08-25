#pragma once
#include "LuaBridge/LuaBridge.h"  
class CommonalityPort :public SOLPort {
public: 
	void InitCalcRunningTime(sol::table solTable);
	 
	void InitTable(sol::table table) override;
};
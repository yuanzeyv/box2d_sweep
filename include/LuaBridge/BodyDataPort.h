#pragma once
#include "LuaBridge/LuaBridge.h" 
class BodyDataPort :public SOLPort {
public: 
	void InitBodyData(sol::table solTable);
	void InitBodyMap(sol::table solTable);
	void InitTable(sol::table table) override;
};
#pragma once
#include "LuaBridge/LuaBridge.h" 
class DelabellaPort :public SOLPort {
public:  
	void InitTable(sol::table table) override;
};
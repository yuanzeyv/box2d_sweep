#pragma once
#include "LuaBridge/LuaBridge.h" 
class B2MathPort :public SOLPort {
public:  
	void Initb2Vec2(sol::table solTable);
	void InitTable(sol::table solTable) override;
};
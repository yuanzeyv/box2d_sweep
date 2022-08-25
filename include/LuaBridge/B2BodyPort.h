#pragma once
#include "LuaBridge/LuaBridge.h" 
class B2BodyPort :public SOLPort {
public:
	void InitB2BodyDef(sol::table solTable);
	void  InitB2Body(sol::table solTable);
	void InitTable(sol::table solTable) override;
};
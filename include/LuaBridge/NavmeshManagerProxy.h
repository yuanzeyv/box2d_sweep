#pragma once
#include "LuaBridge/LuaBridge.h" 
class NavmeshManagerProxy :public SOLPort {
public: 
	void InitNavmeshManager(sol::table solTable); 
	void InitTable(sol::table solTable) override;
};
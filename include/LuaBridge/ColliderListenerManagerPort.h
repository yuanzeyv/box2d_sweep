#pragma once
#include "LuaBridge/LuaBridge.h" 
class ColliderListenerManagerPort :public SOLPort {
public: 
	void InitColliderListenerManager(sol::table solTable);
	  
	void InitTable(sol::table table) override;
};
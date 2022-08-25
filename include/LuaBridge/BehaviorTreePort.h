#pragma once
#include "LuaBridge/LuaBridge.h" 
class BehaviorTreePort :public SOLPort {
public:
	void InitBehaviorTreeFactory(sol::table solTable);
	void InitBTTree(sol::table solTable);
	void InitActionNodeBase(sol::table solTable);
	void InitLuaCoroAction(sol::table solTable);
	void InitConditionNode(sol::table solTable); 
	void InitLuaConditionNode(sol::table solTable);
	void InitTable(sol::table solTable) override;
};
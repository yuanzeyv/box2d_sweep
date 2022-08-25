#pragma once
#include "Library/sol/sol.hpp" 
#include <vector>
class SOLPort {
public:
	virtual void InitTable(sol::table table) = 0;
};
class LuaBridge{
public:
	sol::state LuaState;//����һ��Lua״̬��
	std::vector<SOLPort*> SOLPortList;
	LuaBridge(); 
	void RegisterSolPort(SOLPort* port);
}; 
inline void LuaBridge::RegisterSolPort(SOLPort* port)
{
	SOLPortList.push_back(port);
}
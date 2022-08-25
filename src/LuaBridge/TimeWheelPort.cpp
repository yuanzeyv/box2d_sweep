#include "LuaBridge/TimeWheelPort.h"
#include "Manager/TimeWheelManager.h" 
#include <iostream>
#include <string>  

void TimeWheelPort::InitTimeWheelEvecntPort(sol::table solTable)
{
	auto LuaTimeEvecntTable = solTable.new_usertype<LuaTimeEvecnt>("LuaTimeEvecnt", sol::constructors<LuaTimeEvecnt(sol::table)>());
	LuaTimeEvecntTable["scheduled_at"] = &LuaTimeEvecnt::scheduled_at;//获取到当前的运行时刻
	LuaTimeEvecntTable["active"] = &LuaTimeEvecnt::active;//当前定时器是否激活
	LuaTimeEvecntTable["cancel"] = &LuaTimeEvecnt::cancel;//取消当前的定时器  
}

void TimeWheelPort::InitTimeWheelManagerPort(sol::table solTable)
{
	auto TimeWheelManagerTable = solTable.new_usertype<TimeWheelManager>("TimeWheelMan");
	TimeWheelManagerTable["Instance"] = &TimeWheelManager::Instance; 
	TimeWheelManagerTable["AddTime"] = sol::overload([](TimeWheelManager* self, CTimeEvecnt& timeEvecnt,double time) {
			self->AddTime(timeEvecnt,time); 
		}, 
		[](TimeWheelManager* self, LuaTimeEvecnt& timeEvecnt, double time) {
				self->AddTime(timeEvecnt, time);
		});
} 
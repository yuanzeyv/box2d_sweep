#include "LuaBridge/TimeWheelPort.h"
#include "Manager/TimeWheelManager.h" 
#include <iostream>
#include <string>  

void TimeWheelPort::InitTimeWheelEvecntPort(sol::table solTable)
{
	auto LuaTimeEvecntTable = solTable.new_usertype<LuaTimeEvecnt>("LuaTimeEvecnt", sol::constructors<LuaTimeEvecnt(sol::table)>());
	LuaTimeEvecntTable["scheduled_at"] = &LuaTimeEvecnt::scheduled_at;//��ȡ����ǰ������ʱ��
	LuaTimeEvecntTable["active"] = &LuaTimeEvecnt::active;//��ǰ��ʱ���Ƿ񼤻�
	LuaTimeEvecntTable["cancel"] = &LuaTimeEvecnt::cancel;//ȡ����ǰ�Ķ�ʱ��  
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
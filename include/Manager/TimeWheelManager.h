#pragma once  
#include "Library/TimeWheel/timer-wheel.h" 
#include "Library/sol/sol.hpp" 
class CTimeEvecnt :public TimerEventInterface{
public: 
	CTimeEvecnt():TimerEventInterface(){
	}
	void AddTime(TimerWheel& timers,Tick millisecond) { 
	    timers.schedule(this,millisecond);
	}
	virtual void execute() {
	
	}
};
//lua中的方法 
class LuaTimeEvecnt:public CTimeEvecnt{
public:
	LuaTimeEvecnt(sol::table table):CTimeEvecnt()
	{
		LuaTable = table;
	}
	virtual void execute() {
		LuaTable["Execute"](this);//执行当前的lua回调
	} 
public:
    sol::table LuaTable;//用于存储lua方法 
}; 

class TimeWheelManager
{
public:
	TimerWheel m_TimeWheel; 

	inline void AddTime(CTimeEvecnt& eventTime,double second)
	{
		eventTime.AddTime(this->m_TimeWheel, (Tick)(second * 1000));
	}  

	inline void Update(double tick)//每秒更新函数
	{
		m_TimeWheel.advance((Tick)(tick * 1000));//程序中传入秒,由程序自行转换为 毫秒
	}
	//单例对象句柄
	inline static TimeWheelManager& Instance();
private:
	TimeWheelManager() :m_TimeWheel(0) {

	}
}; 
 
inline TimeWheelManager& TimeWheelManager::Instance() {
	static TimeWheelManager Instance;
	return Instance;
}
 
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
//lua�еķ��� 
class LuaTimeEvecnt:public CTimeEvecnt{
public:
	LuaTimeEvecnt(sol::table table):CTimeEvecnt()
	{
		LuaTable = table;
	}
	virtual void execute() {
		LuaTable["Execute"](this);//ִ�е�ǰ��lua�ص�
	} 
public:
    sol::table LuaTable;//���ڴ洢lua���� 
}; 

class TimeWheelManager
{
public:
	TimerWheel m_TimeWheel; 

	inline void AddTime(CTimeEvecnt& eventTime,double second)
	{
		eventTime.AddTime(this->m_TimeWheel, (Tick)(second * 1000));
	}  

	inline void Update(double tick)//ÿ����º���
	{
		m_TimeWheel.advance((Tick)(tick * 1000));//�����д�����,�ɳ�������ת��Ϊ ����
	}
	//����������
	inline static TimeWheelManager& Instance();
private:
	TimeWheelManager() :m_TimeWheel(0) {

	}
}; 
 
inline TimeWheelManager& TimeWheelManager::Instance() {
	static TimeWheelManager Instance;
	return Instance;
}
 
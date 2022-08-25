#include "LuaBridge/CommonalityPort.h"  
#include<chrono>     
void CommonalityPort::InitCalcRunningTime(sol::table solTable)
{
	solTable["CalcRunningTime"] = [](sol::function func)->int64_t  {
		auto t1 = std::chrono::high_resolution_clock::now();
		func();
		auto t2 = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	};
}
 
void CommonalityPort::InitTable(sol::table table) 
{
	InitCalcRunningTime(table);
}  
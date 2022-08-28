#pragma once
#include "Library/PureMVC/PureMVC.hpp" 
#include "Logic/Proxy/BaseProxy.hpp"
#include "Library/TimeWheel/timer-wheel.h" 
#include "Library/sol/sol.hpp" 
class BehaviorTreeProxy : public BaseProxy

{
public:

	BehaviorTreeProxy() :BaseProxy("BehavioTreeProxy")
	{
	}
};
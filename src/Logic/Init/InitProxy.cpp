#pragma once
#include "iostream"
#include "PureMVC/PureMVC.hpp"
#include "Logic/Proxy/BehaviorTreeProxy.hpp"
#include "Logic/Proxy/TimeWheelProxy.hpp"
#include "Logic/Proxy/PlayeProxy.hpp"
#include "Logic/Proxy/NavmeshProxy.hpp"
#include "Logic/Proxy/LuaLogicProxy.hpp"
#include "Logic/Proxy/DistanceProxy.hpp"
#include "Logic/Proxy/ColliderListenerProxy.hpp"
#include "Logic/Proxy/BodyProxy.hpp" 
using namespace std;
using namespace PureMVC;
using namespace PureMVC::Patterns; 
void InitProxy(IFacade& facade)
{
	facade.registerProxy(new BehaviorTreeProxy());
	facade.registerProxy(new TimeWheelProxy());
	facade.registerProxy(new PlayerProxy());
	facade.registerProxy(new NavmeshProxy());
	facade.registerProxy(new LuaLogicProxy());
	facade.registerProxy(new DistanceProxy());
	facade.registerProxy(new ColliderListenerProxy());
	facade.registerProxy(new BodyProxy());
}
#pragma once
#include "iostream"
#include "PureMVC/PureMVC.hpp"
#include "Logic/Init/InitProxy.hpp"
#include "Logic/Init/InitMediator.hpp"
#include "Logic/Init/InitCommand.hpp"

#include "Logic/Proxy/TimeWheelProxy.hpp"
using namespace std;
using namespace PureMVC;
using namespace PureMVC::Patterns; 
void InitProxy(IFacade& facade)
{ 
	facade.registerProxy(new TimeWheelProxy());//×¢²áËü
}
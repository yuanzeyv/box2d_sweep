#pragma once
#include "iostream"
#include "Library/PureMVC/PureMVC.hpp"
#include "Logic/Init/InitProxy.hpp"
#include "Logic/Init/InitMediator.hpp"
#include "Logic/Init/InitCommand.hpp"
using namespace std;
using namespace PureMVC;
using namespace PureMVC::Patterns;
class MVCInit {
public:
	string m_InstanceName;  
	MVCInit(string facadeName):m_InstanceName(facadeName)
	{
		IFacade& facade =  Facade::getInstance(facadeName); //初始化这个 
		InitCommand(facade);
		InitMediator(facade);
		InitProxy(facade);
	}
};
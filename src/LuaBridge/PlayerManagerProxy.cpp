#include "LuaBridge/PlayerManagerProxy.h"
#include "Manager/PlayerManager.h"
#include <string>
#include <iostream>
#include<fstream>//操作文件的头文件 
void PlayerManagerProxy::InitPlayerManagerPort(sol::table solTable)
{
    auto PlayerManagerPortTable = solTable.new_usertype<PlayerManager>("PlayerMan");
	PlayerManagerPortTable["RegisterPlayer"] = &PlayerManager::RegisterPlayer;
	PlayerManagerPortTable["PlayerEnterGame"] = &PlayerManager::PlayerEnterGame;
	PlayerManagerPortTable["PoPWaitPlayer"] = &PlayerManager::PoPWaitPlayer;
	PlayerManagerPortTable["Instance"] = &PlayerManager::Instance;
} 
void PlayerManagerProxy::InitPlayerBodyData(sol::table solTable)
{
	auto PlayerDataTable = solTable.new_usertype<PlayerData>("PlayerData");
	PlayerDataTable["m_Account"] = &PlayerData::m_Account;
	PlayerDataTable["m_Name"] = &PlayerData::m_Name;
	PlayerDataTable["m_PlayerDesc"] = &PlayerData::m_PlayerDesc;
	PlayerDataTable["SetBodyData"] = &PlayerData::SetBodyData;
  
}
#include "Logic/Proxy/PlayeProxy.hpp"

void PlayerProxy::RegisterPlayer(string account, string name, string playerDesc)//注册一个玩家
{
	PlayerData* playerData = new PlayerData(account, name, playerDesc);
	if (m_EnterGamePlayers.count(playerData->m_Name)) return;
	m_WaitEnterPalyerList.push_back(playerData);
}
void PlayerProxy::PlayerEnterGame(PlayerData* playerData)//玩家登入游戏
{
	if (m_EnterGamePlayers.count(playerData->m_Name)) return;
	m_EnterGamePlayers[playerData->m_Name] = playerData;
}
PlayerData* PlayerProxy::PoPWaitPlayer()//弹出一个待登入的角色信息
{
	if (m_WaitEnterPalyerList.size() == 0) return NULL;
	PlayerData* playerData = *m_WaitEnterPalyerList.begin();
	m_WaitEnterPalyerList.erase(m_WaitEnterPalyerList.begin());
	return playerData;
}
b2Body* PlayerProxy::GetChooseBody()
{
	return m_EnterGamePlayers.begin()->second->m_BodyData;
}
PlayerProxy::PlayerProxy():BaseProxy("PlayerProxy")
{
} 
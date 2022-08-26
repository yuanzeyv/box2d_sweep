//正常流程是,玩家登录后,将角色的详细信息传给
#pragma once  
#include  "Navmesh/Navmesh.h" 
#include <unordered_map>
#include <unordered_set>
#include <vector> 
#include <string>
#include <list> 
#include <iostream> 
#include "Manager/Base/BodyData.h"
#include "Box2d/box2d.h"
using std::unordered_map;
using std::unordered_set;
using std::vector;
using std::string;
typedef int64_t PlayerID;
class PlayerData {//玩家的信息
public:
	string m_Account;//玩家的账号ID
	string m_Name;//玩家有姓名
	string m_PlayerDesc;//玩家的描述信息   
	b2Body* m_BodyData;//玩家关联的刚体信息
	PlayerData(string account, string name, string playerDesc):m_Account(account), m_Name(name), m_PlayerDesc(playerDesc)
	{ 
	}
	void SetBodyData(b2Body* bodyData)
	{
		printf("AAAAAAAAAAc1s9as81c9a81c98a1sc98asc\n\r");
		m_BodyData = bodyData;
	}
}; 
class PlayerManager
{
private: 
	vector<PlayerData*> m_WaitEnterPalyerList;//当前待登录的玩家
	unordered_map<string, PlayerData*> m_EnterGamePlayers;//已经登录过的玩家

	//当前选中的玩家
public: 
	void RegisterPlayer(string account, string name, string playerDesc)//注册一个玩家
	{ 
		PlayerData* playerData = new PlayerData(account,name,playerDesc);
		if (m_EnterGamePlayers.count(playerData->m_Name)) return; 
		m_WaitEnterPalyerList.push_back(playerData);
	}
	void PlayerEnterGame(PlayerData* playerData)//玩家登入游戏
	{
		if (m_EnterGamePlayers.count(playerData->m_Name)) return;
		m_EnterGamePlayers[playerData->m_Name] = playerData;
	}
	PlayerData* PoPWaitPlayer()//弹出一个待登入的角色信息
	{
		if (m_WaitEnterPalyerList.size() == 0) return NULL;
		PlayerData*  playerData =  *m_WaitEnterPalyerList.begin();
		m_WaitEnterPalyerList.erase(m_WaitEnterPalyerList.begin());
		return playerData;
	}
	b2Body* GetChooseBody()
	{ 
		return m_EnterGamePlayers.begin()->second->m_BodyData;
	}
	//单例对象句柄
	inline static PlayerManager& Instance();
private:
	PlayerManager()
	{ 
	}
};  
inline PlayerManager& PlayerManager::Instance() {
	static PlayerManager Instance;
	return Instance;
}
 
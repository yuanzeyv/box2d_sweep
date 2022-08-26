//����������,��ҵ�¼��,����ɫ����ϸ��Ϣ����
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
class PlayerData {//��ҵ���Ϣ
public:
	string m_Account;//��ҵ��˺�ID
	string m_Name;//���������
	string m_PlayerDesc;//��ҵ�������Ϣ   
	b2Body* m_BodyData;//��ҹ����ĸ�����Ϣ
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
	vector<PlayerData*> m_WaitEnterPalyerList;//��ǰ����¼�����
	unordered_map<string, PlayerData*> m_EnterGamePlayers;//�Ѿ���¼�������

	//��ǰѡ�е����
public: 
	void RegisterPlayer(string account, string name, string playerDesc)//ע��һ�����
	{ 
		PlayerData* playerData = new PlayerData(account,name,playerDesc);
		if (m_EnterGamePlayers.count(playerData->m_Name)) return; 
		m_WaitEnterPalyerList.push_back(playerData);
	}
	void PlayerEnterGame(PlayerData* playerData)//��ҵ�����Ϸ
	{
		if (m_EnterGamePlayers.count(playerData->m_Name)) return;
		m_EnterGamePlayers[playerData->m_Name] = playerData;
	}
	PlayerData* PoPWaitPlayer()//����һ��������Ľ�ɫ��Ϣ
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
	//����������
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
 
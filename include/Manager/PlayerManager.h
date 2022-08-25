//����������,��ҵ�¼��,����ɫ����ϸ��Ϣ����
#pragma once  
#include  "Navmesh/Navmesh.h" 
#include <unordered_map>
#include <unordered_set>
#include <vector> 
#include <string>
#include <iostream> 
#include "Box2d/box2d.h"
using std::unordered_map;
using std::unordered_set;
using std::string;
typedef int64_t PlayerID;
class PlayerData {//��ҵ���Ϣ
public:
	string Name;//���������
	//
}; 
class PlayerManager
{
private:
	static PlayerID PlayerAllocID;
	unordered_set //��¼�����
	unordered_set<PlayerID> PlayerList;//ӵ��һ����ҵ�set,�洢���еĽ�ɫ  
public: 
	//����������
	inline static NavmeshManager& Instance();
private:
	PlayerManager():PlayerList(100)
	{ 
	}
}; 
static PlayerManager::PlayerAllocID = 0;
inline PlayerManager& PlayerManager::Instance() {
	static PlayerManager Instance;
	return Instance;
}
 
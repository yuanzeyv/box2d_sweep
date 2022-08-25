//正常流程是,玩家登录后,将角色的详细信息传给
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
class PlayerData {//玩家的信息
public:
	string Name;//玩家有姓名
	//
}; 
class PlayerManager
{
private:
	static PlayerID PlayerAllocID;
	unordered_set //登录的玩家
	unordered_set<PlayerID> PlayerList;//拥有一个玩家的set,存储所有的角色  
public: 
	//单例对象句柄
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
 
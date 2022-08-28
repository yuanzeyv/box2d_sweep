#pragma once 
#include "Logic/Proxy/BaseProxy.hpp"  
#include  "Navmesh/Navmesh.h" 
#include <map>  
#include <vector> 
#include <string>
#include <iostream> 
#include "Box2d/box2d.h"
using std::map;
using std::vector;
using std::string;
typedef int64_t NavmeshID; 
class NavmeshProxy : public BaseProxy
{
private:
	static NavmeshID AllocNavmeshID;
public:
	map<NavmeshID, Navmesh*> NavmeshMap;//寻路map key为地图ID,
	map<ActorID, NavmeshID> MapInner;//哪个角色处于那张地图(后期会将字符串ID变为整形ID)
	map<NavmeshID, map<ActorID, bool>> MapContain;//哪个角色处于那张地图(后期会将字符串ID变为整形ID)
public:
	//生成加载一个navmesh
	NavmeshID GenerateNavmesh(string fileName);//返回一个meshID,后期用这个ID进行寻路地图的维护 
	//删除一个地图
	void RemoveNavmesh(NavmeshID navID);
	//加入一个角色  
	bool RegisterActor(NavmeshID navID, ActorID actorId);
	void UnregisterActor(ActorID actorId);
	//寻找一个随机寻路点
	bool FindRandPoint(NavmeshID navID, PosV3& pos);
	//寻路
	void FindPath(int64_t actorID, b2Vec2 startPos, b2Vec2 endPos, bool soomth = false);//开始位置 结束位置,是否平滑路径
	//射线查询
	float Recast(ActorID actorID, b2Vec2 startPos, b2Vec2 endPos);//开始位置 结束位置
	//分片寻路,好了之后会进行通知
	void SlicedFindPath(ActorID actorID, b2Vec2 startPos, b2Vec2 endPos);
	//用于刷新分片寻路
	void Update(float dt); 

	NavmeshProxy();
};   
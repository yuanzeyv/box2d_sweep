#include "Logic/Proxy/NavmeshProxy.hpp"
#include "Manager/Base/BodyData.h" 
#include "Manager/DistanceManager.h"
#include "PySourceParse/PhysicsParse.h"
#include <vector>
#include <map>  
#include <map>  
#include <vector> 
#include <string>
#include <iostream> 
#include "Box2d/box2d.h"
using std::map;
using std::vector;
using std::string;
using namespace std;   
NavmeshID NavmeshProxy::AllocNavmeshID = 1;
NavmeshProxy::NavmeshProxy():BaseProxy("NavmeshProxy") {
}
//生成加载一个navmesh
NavmeshID NavmeshProxy::GenerateNavmesh(string fileName)//返回一个meshID,后期用这个ID进行寻路地图的维护
{
	Navmesh* navmesh = new Navmesh();
	if (!navmesh->GenerateNavMesh("./" + fileName + ".obj")) {
		delete navmesh;
		return 0;
	}
	NavmeshID navID = AllocNavmeshID++;
	NavmeshMap[navID] = navmesh;
	return navID;
}
//删除一个地图
void NavmeshProxy::RemoveNavmesh(NavmeshID navID)
{
	if (NavmeshMap.find(navID) == NavmeshMap.end())//对于地图不存在
		return;
	if (MapContain.find(navID) == MapContain.end())//对应map从未添加过角色
		return;
	for (auto item = MapContain[navID].begin(); item != MapContain[navID].end();) {//循环遍历每个map里面的角色信息
		MapInner.erase(item->first);//删除mapInner表中的记录
		NavmeshMap[navID]->BreakActorSliced(item->first);//调用Navmesh的删除函数
		delete NavmeshMap[navID];
		NavmeshMap.erase(navID);
	}
	MapContain.erase(navID);//删除
}
//加入一个角色
bool NavmeshProxy::RegisterActor(NavmeshID navID, ActorID actorId)
{
	if (NavmeshMap.find(navID) == NavmeshMap.end())
		return false;
	if (MapInner.find(actorId) != MapInner.end())//如果当前已经加入了地图
		return false;
	MapInner[actorId] = navID;
	MapContain[navID][actorId] = true;//将角色注册入其中
	return true;
}
void NavmeshProxy::UnregisterActor(ActorID actorId)
{
	if (MapInner.find(actorId) == MapInner.end())//如果当前已经加入了地图
		return;
	NavmeshID navID = MapInner[actorId];
	MapInner.erase(actorId);
	MapContain[navID].erase(actorId);
	NavmeshMap[navID]->BreakActorSliced(actorId);//存在切片遍历的话,删除 
}
//寻路
void NavmeshProxy::FindPath(int64_t actorID, b2Vec2 startPos, b2Vec2 endPos, bool soomth)//开始位置 结束位置,是否平滑路径
{//首先判断当前玩家是否有加入到navmesh中
	if (MapInner.find(actorID) == MapInner.end())//如果当前已经加入了地图
		return;
	NavmeshID navID = MapInner[actorID];
	Navmesh* navmeshCell = NavmeshMap[navID];
	PosV3 start(startPos.x, 0, startPos.y);
	PosV3 end(endPos.x, 0, endPos.y);
	navmeshCell->FindPath(start, end, soomth);
}
//寻找一个随机寻路点
bool NavmeshProxy::FindRandPoint(NavmeshID navID, PosV3& pos)
{
	if (NavmeshMap.find(navID) == NavmeshMap.end())
		return false;
	NavmeshMap[navID]->FindRodomPoint(pos);
	return true;
}
//射线查询
float NavmeshProxy::Recast(ActorID actorID, b2Vec2 startPos, b2Vec2 endPos)//开始位置 结束位置
{	//首先判断当前玩家是否有加入到navmesh中
	if (MapInner.find(actorID) == MapInner.end())//如果当前已经加入了地图
		return 0;
	PosV3 start(startPos.x, 0, startPos.y);
	PosV3 end(endPos.x, 0, endPos.y);
	NavmeshID navID = MapInner[actorID];
	Navmesh* navmeshCell = NavmeshMap[navID];
	return navmeshCell->Recast(start, end);
}
//分片寻路,好了之后会进行通知
void NavmeshProxy::SlicedFindPath(ActorID actorID, b2Vec2 startPos, b2Vec2 endPos)
{	//首先判断当前玩家是否有加入到navmesh中
	if (MapInner.find(actorID) == MapInner.end())//如果当前已经加入了地图
		return;
	PosV3 start(startPos.x, 0, startPos.y);
	PosV3 end(endPos.x, 0, endPos.y);
	NavmeshID navID = MapInner[actorID];
	Navmesh* navmeshCell = NavmeshMap[navID];
	navmeshCell->FindPath_Sliced(actorID, start, end);
}
//用于刷新分片寻路
void NavmeshProxy::Update(float dt)
{
	static int i = 0;
	static float  time = 0;
	time += dt;
	for (auto item = NavmeshMap.begin(); item != NavmeshMap.end(); item++)
	{
		item->second->Update(dt);
	}
}

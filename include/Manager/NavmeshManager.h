//NavmeshManager& data = NavmeshManager::Instance();
//NavmeshID id = data.GenerateNavmesh("a");
//for (int i = 0; i < 200;i++)
//{
//	data.RegisterActor(id, i);
//} 
//vector<PosV3> startRand;
//vector<PosV3> endRand;
//for (int i = 0; i < 200;i++)
//{
//	startRand.push_back(PosV3());
//	endRand.push_back(PosV3()); 
//	data.FindRandPoint(id, startRand[i]);
//	data.FindRandPoint(id, endRand[i]);
//} 
//auto t1 = chrono::high_resolution_clock::now();
//for (int i = 0; i < 200;i++)
//{
//	//data.Recast(i, b2Vec2(startRand[i].X, startRand[i].Z), b2Vec2(endRand[i].X, endRand[i].Z));
// 	//data.FindPath(i, b2Vec2(startRand[i].X, startRand[i].Z), b2Vec2(endRand[i].X, endRand[i].Z)); 
//	data.SlicedFindPath(i, b2Vec2(startRand[i % 3 ].X, startRand[i %3].Z), b2Vec2(endRand[i%3 ].X, endRand[i%3 ].Z));
//}
//auto t2 = chrono::high_resolution_clock::now();
//long long int  a = chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count(); 
//printf("执行5000次射线的时间为:%lld\n\r", a);
#pragma once  
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
class NavmeshManager
{
private:
	static NavmeshID AllocNavmeshID;
public:  
	map<NavmeshID, Navmesh*> NavmeshMap;//寻路map key为地图ID,
	map<ActorID, NavmeshID> MapInner;//哪个角色处于那张地图(后期会将字符串ID变为整形ID)
	map<NavmeshID, map<ActorID,bool>> MapContain;//哪个角色处于那张地图(后期会将字符串ID变为整形ID)
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
	//单例对象句柄
	inline static NavmeshManager& Instance();
private:
	NavmeshManager();
}; 
 
inline NavmeshManager& NavmeshManager::Instance() {
	static NavmeshManager Instance;
	return Instance;
}
 
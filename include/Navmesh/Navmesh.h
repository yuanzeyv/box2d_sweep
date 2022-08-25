#pragma once
#include "Recast/Recast.h"
#include "DetourTileCache/DetourTileCache.h"
#include "DetourTileCache/DetourTileCacheBuilder.h"
#include "Detour/DetourNavMeshQuery.h"
#include "Navmesh/InputGeom.h"
#include "Navmesh/SampleInterfaces.h"  
#include "DetourTileCache/DetourTileCache.h"
#include "Detour/DetourNavMesh.h"
#include "Navmesh/RegionParam.h"
#include "Navmesh/NavmeshBuilder.h"
#include <chrono>  
#include <map>
#include <stack>
#include "Box2d/box2d.h"
#include <stdint.h>
using std::map;
using std::stack;  
#define MAX_POLYS 500 //最多几个形状
#define MAX_SMOOTH 2048 //平滑路线     
//寻找路径的一个结构体
class FindWayFalt {
public:
	PosV3 StartPos;
	PosV3 EndPos;
	dtPolyRef StartRef;
	dtPolyRef EndRef;
	dtPolyRef PathPolys[MAX_POLYS];;
	int PathPolysCount;
	dtNavMeshQuery* MeshQuery;
	dtStatus PathFindStatus;//当前的寻路状态 
	FindWayFalt(); 
	FindWayFalt(dtNavMesh* navmesh);
	~FindWayFalt();
	inline bool SetNavMesh(dtNavMesh* navmesh);

};

class Navmesh { 
public:
	RegionParam RegionData;//加载网格需要传入的参数

	NavmeshBuilder* NavmeshBuild; 

	BuildContext Ctx;//日志结构 
	dtNavMesh* NavMeshCell;  
	dtQueryFilter Filter;//感觉想像是路径消耗 
	PosV3 PolyPickExt;//周边的点位

	//用于寻路的结构 直接寻路 与 射线
	FindWayFalt NavQueryCell;


	float SmoothPathArray[MAX_SMOOTH * 3];//平滑的路线
	int SmoothPathCount;

	//分片寻路的结构
	map<int64_t, FindWayFalt*> SlicedQueryMap; //当前正在寻找中的分片 
	map<int64_t, FindWayFalt*> SlicedFinishTable; //分片完成表
	map<int64_t, FindWayFalt*> SlicedUnFinishTable; //分片完成表
	stack<FindWayFalt*> IdleQueryVector; //保存废弃的,时刻准备再利用(每一分钟将进行一次清理) 
	  
public:   
	bool GenerateNavMesh(string path);


	float Recast(PosV3& startPos, PosV3& endPos);//检查射线
	bool FindPath(PosV3& startPos, PosV3& endPos, bool sommthPath = false);
	bool SoomthPaht();//将当前路径转换为平滑路径
	bool FindRodomPoint(PosV3& startPos);//寻找一个随机点

	bool FindPath_Sliced(ActorID actorID, PosV3& startPos, PosV3& endPos);//切片寻路 
	bool Update_FindPath_Sliced(ActorID actorID, FindWayFalt* findWayFalt);//切片寻路更新
	FindWayFalt* GetIdleFindWayFalt();//获取一个空闲的对象
	void BreakActorSliced(ActorID id);//中断一个查询  
	//更新函数
	void Update(float dt);
	Navmesh(); 
	virtual ~Navmesh();
};         


inline bool FindWayFalt::SetNavMesh(dtNavMesh* navmesh)
{
	MeshQuery->init(navmesh, 2048);
	return true;
}
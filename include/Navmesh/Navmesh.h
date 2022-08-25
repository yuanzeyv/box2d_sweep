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
#define MAX_POLYS 500 //��༸����״
#define MAX_SMOOTH 2048 //ƽ��·��     
//Ѱ��·����һ���ṹ��
class FindWayFalt {
public:
	PosV3 StartPos;
	PosV3 EndPos;
	dtPolyRef StartRef;
	dtPolyRef EndRef;
	dtPolyRef PathPolys[MAX_POLYS];;
	int PathPolysCount;
	dtNavMeshQuery* MeshQuery;
	dtStatus PathFindStatus;//��ǰ��Ѱ·״̬ 
	FindWayFalt(); 
	FindWayFalt(dtNavMesh* navmesh);
	~FindWayFalt();
	inline bool SetNavMesh(dtNavMesh* navmesh);

};

class Navmesh { 
public:
	RegionParam RegionData;//����������Ҫ����Ĳ���

	NavmeshBuilder* NavmeshBuild; 

	BuildContext Ctx;//��־�ṹ 
	dtNavMesh* NavMeshCell;  
	dtQueryFilter Filter;//�о�������·������ 
	PosV3 PolyPickExt;//�ܱߵĵ�λ

	//����Ѱ·�Ľṹ ֱ��Ѱ· �� ����
	FindWayFalt NavQueryCell;


	float SmoothPathArray[MAX_SMOOTH * 3];//ƽ����·��
	int SmoothPathCount;

	//��ƬѰ·�Ľṹ
	map<int64_t, FindWayFalt*> SlicedQueryMap; //��ǰ����Ѱ���еķ�Ƭ 
	map<int64_t, FindWayFalt*> SlicedFinishTable; //��Ƭ��ɱ�
	map<int64_t, FindWayFalt*> SlicedUnFinishTable; //��Ƭ��ɱ�
	stack<FindWayFalt*> IdleQueryVector; //���������,ʱ��׼��������(ÿһ���ӽ�����һ������) 
	  
public:   
	bool GenerateNavMesh(string path);


	float Recast(PosV3& startPos, PosV3& endPos);//�������
	bool FindPath(PosV3& startPos, PosV3& endPos, bool sommthPath = false);
	bool SoomthPaht();//����ǰ·��ת��Ϊƽ��·��
	bool FindRodomPoint(PosV3& startPos);//Ѱ��һ�������

	bool FindPath_Sliced(ActorID actorID, PosV3& startPos, PosV3& endPos);//��ƬѰ· 
	bool Update_FindPath_Sliced(ActorID actorID, FindWayFalt* findWayFalt);//��ƬѰ·����
	FindWayFalt* GetIdleFindWayFalt();//��ȡһ�����еĶ���
	void BreakActorSliced(ActorID id);//�ж�һ����ѯ  
	//���º���
	void Update(float dt);
	Navmesh(); 
	virtual ~Navmesh();
};         


inline bool FindWayFalt::SetNavMesh(dtNavMesh* navmesh)
{
	MeshQuery->init(navmesh, 2048);
	return true;
}
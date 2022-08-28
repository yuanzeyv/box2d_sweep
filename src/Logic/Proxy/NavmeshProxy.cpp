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
//���ɼ���һ��navmesh
NavmeshID NavmeshProxy::GenerateNavmesh(string fileName)//����һ��meshID,���������ID����Ѱ·��ͼ��ά��
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
//ɾ��һ����ͼ
void NavmeshProxy::RemoveNavmesh(NavmeshID navID)
{
	if (NavmeshMap.find(navID) == NavmeshMap.end())//���ڵ�ͼ������
		return;
	if (MapContain.find(navID) == MapContain.end())//��Ӧmap��δ��ӹ���ɫ
		return;
	for (auto item = MapContain[navID].begin(); item != MapContain[navID].end();) {//ѭ������ÿ��map����Ľ�ɫ��Ϣ
		MapInner.erase(item->first);//ɾ��mapInner���еļ�¼
		NavmeshMap[navID]->BreakActorSliced(item->first);//����Navmesh��ɾ������
		delete NavmeshMap[navID];
		NavmeshMap.erase(navID);
	}
	MapContain.erase(navID);//ɾ��
}
//����һ����ɫ
bool NavmeshProxy::RegisterActor(NavmeshID navID, ActorID actorId)
{
	if (NavmeshMap.find(navID) == NavmeshMap.end())
		return false;
	if (MapInner.find(actorId) != MapInner.end())//�����ǰ�Ѿ������˵�ͼ
		return false;
	MapInner[actorId] = navID;
	MapContain[navID][actorId] = true;//����ɫע��������
	return true;
}
void NavmeshProxy::UnregisterActor(ActorID actorId)
{
	if (MapInner.find(actorId) == MapInner.end())//�����ǰ�Ѿ������˵�ͼ
		return;
	NavmeshID navID = MapInner[actorId];
	MapInner.erase(actorId);
	MapContain[navID].erase(actorId);
	NavmeshMap[navID]->BreakActorSliced(actorId);//������Ƭ�����Ļ�,ɾ�� 
}
//Ѱ·
void NavmeshProxy::FindPath(int64_t actorID, b2Vec2 startPos, b2Vec2 endPos, bool soomth)//��ʼλ�� ����λ��,�Ƿ�ƽ��·��
{//�����жϵ�ǰ����Ƿ��м��뵽navmesh��
	if (MapInner.find(actorID) == MapInner.end())//�����ǰ�Ѿ������˵�ͼ
		return;
	NavmeshID navID = MapInner[actorID];
	Navmesh* navmeshCell = NavmeshMap[navID];
	PosV3 start(startPos.x, 0, startPos.y);
	PosV3 end(endPos.x, 0, endPos.y);
	navmeshCell->FindPath(start, end, soomth);
}
//Ѱ��һ�����Ѱ·��
bool NavmeshProxy::FindRandPoint(NavmeshID navID, PosV3& pos)
{
	if (NavmeshMap.find(navID) == NavmeshMap.end())
		return false;
	NavmeshMap[navID]->FindRodomPoint(pos);
	return true;
}
//���߲�ѯ
float NavmeshProxy::Recast(ActorID actorID, b2Vec2 startPos, b2Vec2 endPos)//��ʼλ�� ����λ��
{	//�����жϵ�ǰ����Ƿ��м��뵽navmesh��
	if (MapInner.find(actorID) == MapInner.end())//�����ǰ�Ѿ������˵�ͼ
		return 0;
	PosV3 start(startPos.x, 0, startPos.y);
	PosV3 end(endPos.x, 0, endPos.y);
	NavmeshID navID = MapInner[actorID];
	Navmesh* navmeshCell = NavmeshMap[navID];
	return navmeshCell->Recast(start, end);
}
//��ƬѰ·,����֮������֪ͨ
void NavmeshProxy::SlicedFindPath(ActorID actorID, b2Vec2 startPos, b2Vec2 endPos)
{	//�����жϵ�ǰ����Ƿ��м��뵽navmesh��
	if (MapInner.find(actorID) == MapInner.end())//�����ǰ�Ѿ������˵�ͼ
		return;
	PosV3 start(startPos.x, 0, startPos.y);
	PosV3 end(endPos.x, 0, endPos.y);
	NavmeshID navID = MapInner[actorID];
	Navmesh* navmeshCell = NavmeshMap[navID];
	navmeshCell->FindPath_Sliced(actorID, start, end);
}
//����ˢ�·�ƬѰ·
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

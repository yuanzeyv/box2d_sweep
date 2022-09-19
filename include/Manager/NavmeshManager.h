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
//printf("ִ��5000�����ߵ�ʱ��Ϊ:%lld\n\r", a);
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
	map<NavmeshID, Navmesh*> NavmeshMap;//Ѱ·map keyΪ��ͼID,
	map<ActorID, NavmeshID> MapInner;//�ĸ���ɫ�������ŵ�ͼ(���ڻὫ�ַ���ID��Ϊ����ID)
	map<NavmeshID, map<ActorID,bool>> MapContain;//�ĸ���ɫ�������ŵ�ͼ(���ڻὫ�ַ���ID��Ϊ����ID)
public:
	//���ɼ���һ��navmesh
	NavmeshID GenerateNavmesh(string fileName);//����һ��meshID,���������ID����Ѱ·��ͼ��ά�� 
	//ɾ��һ����ͼ
	void RemoveNavmesh(NavmeshID navID);
	//����һ����ɫ
	bool RegisterActor(NavmeshID navID, ActorID actorId);
	void UnregisterActor(ActorID actorId);
	//Ѱ��һ�����Ѱ·��
	bool FindRandPoint(NavmeshID navID, PosV3& pos);
	//Ѱ·
	void FindPath(int64_t actorID, b2Vec2 startPos, b2Vec2 endPos, bool soomth = false);//��ʼλ�� ����λ��,�Ƿ�ƽ��·��
	//���߲�ѯ
	float Recast(ActorID actorID, b2Vec2 startPos, b2Vec2 endPos);//��ʼλ�� ����λ��
	//��ƬѰ·,����֮������֪ͨ
	void SlicedFindPath(ActorID actorID, b2Vec2 startPos, b2Vec2 endPos);
	//����ˢ�·�ƬѰ·
	void Update(float dt); 
	//����������
	inline static NavmeshManager& Instance();
private:
	NavmeshManager();
}; 
 
inline NavmeshManager& NavmeshManager::Instance() {
	static NavmeshManager Instance;
	return Instance;
}
 
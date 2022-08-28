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
	map<NavmeshID, Navmesh*> NavmeshMap;//Ѱ·map keyΪ��ͼID,
	map<ActorID, NavmeshID> MapInner;//�ĸ���ɫ�������ŵ�ͼ(���ڻὫ�ַ���ID��Ϊ����ID)
	map<NavmeshID, map<ActorID, bool>> MapContain;//�ĸ���ɫ�������ŵ�ͼ(���ڻὫ�ַ���ID��Ϊ����ID)
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

	NavmeshProxy();
};   
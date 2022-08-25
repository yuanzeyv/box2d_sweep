#pragma once
#include "Manager/Base/BodyData.h"   
#include "Manager/BodyManager.h" 
#include "Manager/BodyManager.h"
#include "Manager/DistanceManager.h"
#include <map> 
#include <vector>   
using namespace std;  
//ע��һ��bodyData
bool DistanceManager::RegisterBody(BodyData* bodyData) { 
	DistanceObjMap[bodyData->ID()] = bodyData;//���ڴ洢��¼ ������Ϣ
	BodyType type = bodyData->Type();
	if (type == BodyType::MonsterBody) {//�������Ϊ �������͵�λ��
		__RegisterBody(bodyData, ViewType::Monster_Hero, BodyType::PlayerBody,ViewType::Hero_Monster); 
	}
	else if (type == BodyType::PlayerBody)
	{
		__RegisterBody(bodyData, ViewType::Hero_Monster, BodyType::MonsterBody,ViewType::Monster_Hero);
		__RegisterBody(bodyData, ViewType::Hero_Hero, BodyType::PlayerBody, ViewType::Null);
		__RegisterBody(bodyData, ViewType::Hero_Static, BodyType::StaticBody, ViewType::Static_Hero);
		__RegisterBody(bodyData, ViewType::Hero_Bullet, BodyType::BulletBody, ViewType::Bullet_Hero);
	}
	else if (type == BodyType::StaticBody)
		__RegisterBody(bodyData, ViewType::Static_Hero, BodyType::PlayerBody, ViewType::Hero_Static);
	else if (type == BodyType::BulletBody) 
		__RegisterBody(bodyData, ViewType::Bullet_Hero , BodyType::PlayerBody, ViewType::Hero_Bullet); 
	return true;
}
void DistanceManager::UnregisterBody(ActorID id)
{
	UnregisterBody(DistanceObjMap[id]);//��ʼ��ע�������������
}

void DistanceManager::UnregisterBody(BodyData* body)
{
	if (body->Type() == BodyType::MonsterBody) {
		__UnregisterBody(body, ViewType::Monster_Hero,ViewType::Hero_Monster);
	}
	else if (body->Type() == BodyType::PlayerBody)
	{
		__UnregisterBody(body, ViewType::Hero_Monster, ViewType::Monster_Hero);
		__UnregisterBody(body, ViewType::Hero_Hero, ViewType::Null);
		__UnregisterBody(body, ViewType::Hero_Static, ViewType::Static_Hero);
		__UnregisterBody(body, ViewType::Hero_Bullet, ViewType::Bullet_Hero);
	}
	else if (body->Type() == BodyType::StaticBody)
	{
		__UnregisterBody(body, ViewType::Static_Hero, ViewType::Hero_Static);
	}
	else if (body->Type() == BodyType::BulletBody)
	{ 
		__UnregisterBody(body, ViewType::Bullet_Hero, ViewType::Hero_Bullet);
	} 
	IgnoreCalcMap.erase(body->ID());
	DistanceObjMap.erase(body->ID());
	DistanceCalcRemove(body);
}

//��ȡ����Ұ��Χ�� �����һ��BodyData
b2Body* DistanceManager::MinumumDistanceBody(ActorID checkID, ViewType checkType)
{ 
	BodyData* bodyData = DistanceObjMap[checkID];
	auto visibleList = DistanceMap[checkType][checkID][ViewStatus::Visble];
	float distance = 9999;
	b2Body* retBody = NULL;
	b2Vec2 bodyPos = bodyData->GetBody()->GetPosition();
	for (auto begin = visibleList.begin(); begin != visibleList.end(); begin++) {
		b2Body* tempBody = begin->second->GetBody();
		b2Vec2 dir = bodyPos - tempBody->GetPosition();//����ͨ�������� ��ȡ�������һ���� 
		if (dir.Length() > distance && checkID != begin->first) {
			distance = dir.Length();
			retBody = tempBody;
		}
	}
	return retBody;
}
void DistanceManager::RecountDistance(BodyData * body) {
	if (body->Type() == BodyType::MonsterBody) { 
		__RecountDistance(body, ViewType::Monster_Hero, ViewType::Hero_Monster);
	} else if (body->Type() == BodyType::PlayerBody) {
		__RecountDistance(body, ViewType::Hero_Monster, ViewType::Monster_Hero);
		__RecountDistance(body, ViewType::Hero_Hero, ViewType::Null);
		__RecountDistance(body, ViewType::Hero_Static, ViewType::Null);
		__RecountDistance(body, ViewType::Hero_Bullet, ViewType::Bullet_Hero);
	}else if (body->Type() == BodyType::BulletBody) {
		__RecountDistance(body, ViewType::Bullet_Hero, ViewType::Hero_Bullet);
	}
}

//���¼������
void DistanceManager::__RecountDistance(BodyData* body, ViewType viewType, ViewType addType)
{
	auto& inviewMap = DistanceMap[viewType][body->ID()][ViewStatus::Invisble];//Ϊ�˷������ ��ȡ�������ӱ�
	auto& viewMap = DistanceMap[viewType][body->ID()][ViewStatus::Visble];//Ϊ�˷������ ��ȡ�����ӱ�
	map<ActorID, BodyData*> tempInviewMap;//��ʱ�洢���Է����ò���Map�ṹ
	const b2AABB& BodyAABB = body->GetViewAABB();//��ȡ�����ӷ�Χ
	for (auto begin = viewMap.begin(); begin != viewMap.end(); ) {//��ʼ���㿴�ü��ı� 
		if (!b2TestOverlap(BodyAABB, begin->second->GetAABB())) { //�����ǰ��Ĳ�������
			tempInviewMap.insert(*begin);//���뵽��ʱ 
			viewMap.erase(begin++);//����Mapɾ��������
		}
		else {
			begin++;
		}
	}
	for (auto begin = inviewMap.begin(); begin != inviewMap.end(); ) {//�ٴα������ɼ�  
		if (b2TestOverlap(BodyAABB, begin->second->GetAABB())) { //�����ǰ���Ϊ�ɼ��� 
			viewMap.insert(*begin);//���뵽�ɼ�
			inviewMap.erase(begin++);//�Ӳ��ɼ��б���ɾ��
		}
		else {
			begin++;
		}
	}
	inviewMap.insert(tempInviewMap.begin(), tempInviewMap.end());//��������ʱMap���
	if (addType == ViewType::Null)//��ִ��֮��Ĳ���
		goto con;
	{
		auto& visbleMainMap = DistanceMap[viewType][body->ID()][ViewStatus::Visble];//��ȡ�����������������������Ϣ
		auto& typeMap = DistanceMap[addType];//��ȡ�� ���������Map
		for (auto item = typeMap.begin(); item != typeMap.end(); item++) {//��������ı���и���
			ViewStatus isMainVisble = visbleMainMap.find(item->first) != visbleMainMap.end() ? ViewStatus::Visble : ViewStatus::Invisble;//�ж����Ƿ��ڿɼ��б��У��ó��Ľ���ǵ�ǰ���ڵ��б�
			ViewStatus isInverterVisble = isMainVisble == ViewStatus::Visble ? ViewStatus::Invisble : ViewStatus::Visble;//�ж����Ƿ��ڿɼ��б��У��ó��Ľ���ǵ�ǰ���ڵ��б�
			auto relevanceVisibleMap = typeMap[item->first][isMainVisble];//��ȡ����Ӧ�����б��б�
			ViewStatus isRelevanceVisible = relevanceVisibleMap.find(body->ID()) != relevanceVisibleMap.end() ? isMainVisble : isInverterVisble;//�����б��Ƿ��������Ԫ�����û�У�
			if (isMainVisble != isRelevanceVisible) {
				typeMap[item->first][isInverterVisble].erase(body->ID());
				typeMap[item->first][isMainVisble][body->ID()] = body;
			}
		}
	}
con:
	return;
}

//���һ����Ԫ �����е� listType �µĵ�Ԫ���뵽 viewType �£�����BodyData��Ԫ���뵽 addType��
void DistanceManager::__RegisterBody(BodyData* body, ViewType viewType, BodyType listType, ViewType addType)
{
	const b2AABB& BodyAABB = body->GetViewAABB(); //���Ȼ�ȡ���������Ұ��Χ
	DistanceMap[viewType][body->ID()] = unordered_map< ViewStatus, unordered_map<ActorID, BodyData*>>();//���丳ֵ
	auto BodyList = BodyManager::Instance().GetBodyList(listType);//��ȡ������������б� 
	for (auto begin = BodyList.begin(); begin != BodyList.end();begin++){//��ʼ��������б�
		ActorID ID = begin->second->ID();
		BodyData* calcBody = DistanceObjMap[ID];
		ViewStatus viewStatus = b2TestOverlap(BodyAABB, calcBody->GetAABB()) ? ViewStatus::Visble : ViewStatus::Invisble;//����Ƿ�����ڹ�����Ұ��Χ��
		DistanceMap[viewType][body->ID()][viewStatus][begin->first] = calcBody; //�������ӵ���Ӧ��Ұ��Χ
	} 
	if (addType == ViewType::Null)//�Ƿ�������������
		goto con;
	{
		auto& visbleMainMap = DistanceMap[viewType][body->ID()][ViewStatus::Visble];//��ȡ�����������������������Ϣ
		auto& typeMap = DistanceMap[addType];//��ȡ�� ���������Map
		for (auto item = typeMap.begin();item != typeMap.end();item++) {//��������ı���и���
			ViewStatus isMainVisble = visbleMainMap.find(item->first) != visbleMainMap.end() ? ViewStatus::Visble : ViewStatus::Invisble;//�ж����Ƿ��ڿɼ��б��У��ó��Ľ���ǵ�ǰ���ڵ��б�
			ViewStatus isInverterVisble = isMainVisble == ViewStatus::Visble ? ViewStatus::Invisble : ViewStatus::Visble;//�ж����Ƿ��ڿɼ��б��У��ó��Ľ���ǵ�ǰ���ڵ��б�
			auto relevanceVisibleMap = typeMap[item->first][isMainVisble];//��ȡ����Ӧ�����б��б�
			ViewStatus isRelevanceVisible = relevanceVisibleMap.find(body->ID()) != relevanceVisibleMap.end() ? isMainVisble : isInverterVisble;//�����б��Ƿ��������Ԫ�����û�У�
			if (isMainVisble != isRelevanceVisible) {
				typeMap[item->first][isInverterVisble].erase(body->ID());
				typeMap[item->first][isMainVisble][body->ID()] = body;
			}
		}
	} 
con:
	return;
}
//��ע��һ������
void DistanceManager::__UnregisterBody(BodyData* body, ViewType viewType, ViewType addType)
{
	auto viewMap = DistanceMap[viewType][body->ID()];
	auto visibleMap = viewMap[ViewStatus::Visble];
	auto invisibleMap = viewMap[ViewStatus::Invisble];
	if (addType != ViewType::Null)//�Ƿ������������� 
	{	
		//�������Կ����ĳ�Ա
		for (auto begin = visibleMap.begin(); begin != visibleMap.end(); begin++) 
			DistanceMap[addType][begin->first][ViewStatus::Visble].erase(body->ID()); 
		//���������Կ����ĳ�Ա
		for (auto begin = visibleMap.begin(); begin != visibleMap.end(); begin++) 
			DistanceMap[addType][begin->first][ViewStatus::Invisble].erase(body->ID()); 
	}
	DistanceMap[viewType].erase(body->ID());  
}
void DistanceManager::DistanceCalc() {    
	if (!DelayedCalcMap.size())
		return;
	for (auto item = DelayedCalcMap.begin();item != DelayedCalcMap.end();item++)
	{
		BodyData* body = DistanceObjMap[*item];//ͨ��ID��ȡ������������Ϣ
		body->CalcBodyAABB();//���¼��㵱���ǵ�AABB
		body->CalcViewBody();//���¼��㵱ǰ����ͼAABB
		if (IgnoreCalcMap.find(body->ID()) != IgnoreCalcMap.end())//�����ǰλ�÷����˱䶯
			if (b2TestOverlap(body->GetAABB(), IgnoreCalcMap[body->ID()]))//�жϵ�ǰAABB�� ���� ������λ���ص�
				continue;//�����ƶ��˰����λ�������ٴβμӾ������  
		IgnoreCalcMap[body->ID()] = body->GetAABB();//���¼�����λ 
		RecountDistance(body); 
	} 
	DistanceCalcClear();
}


void DistanceManager::DistanceDump(ActorID id) {
	BodyData* bodyData = DistanceObjMap[id];
	if (bodyData->Type() == BodyType::MonsterBody)
	{
		__Dump(id, ViewType::Monster_Hero);
	}
	else if (bodyData->Type() == BodyType::PlayerBody)
	{
		__Dump(id, ViewType::Hero_Hero);
		__Dump(id, ViewType::Hero_Monster);
	}
}
//��ӡ��ǰ�ɼ�����
void DistanceManager::__Dump(ActorID id , ViewType type)
{
	auto visbleMap = DistanceMap[type][id][ViewStatus::Visble];
	auto invisbleMap = DistanceMap[type][id][ViewStatus::Invisble];
	printf("��ǰ%lld�ɼ��Ķ��� : ", id);
	for (auto begin = visbleMap.begin(); begin != visbleMap.end(); begin++) {//��ʼ���㿴�ü��ı�  
		printf("%lld ", begin->first);
	}
	printf("\n\r");
	printf("��ǰ%lld���ɼ��Ķ��� : ", id);
	for (auto begin = invisbleMap.begin(); begin != invisbleMap.end(); begin++) {//��ʼ���㿴�ü��ı�  
		printf("%lld ", begin->first);
	}
	printf("\n\r");
}
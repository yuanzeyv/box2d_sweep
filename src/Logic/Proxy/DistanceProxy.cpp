#include "Logic/Proxy/DistanceProxy.hpp"
#include "Manager/Base/BodyData.h"   
#include "Manager/BodyManager.h"  
#include <map> 
#include <vector>   
using namespace std;
//ע��һ��bodyData
bool DistanceProxy::RegisterBody(BodyData* bodyData) {
	DistanceObjMap[bodyData->ID()] = bodyData;//���ڴ洢��¼ ������Ϣ
	BodyType type = bodyData->Type();
	if (type == BodyType::MONSTER_BODY) {//�������Ϊ �������͵�λ��
		__RegisterBody(bodyData, ViewType::MONSTER_HERO, BodyType::PLAYER_BODY, ViewType::HERO_MONSTER);
	}
	else if (type == BodyType::PLAYER_BODY)
	{
		__RegisterBody(bodyData, ViewType::HERO_MONSTER, BodyType::MONSTER_BODY, ViewType::MONSTER_HERO);
		__RegisterBody(bodyData, ViewType::HERO_HERO, BodyType::PLAYER_BODY, ViewType::VIEW_TYPE_NULL);
		__RegisterBody(bodyData, ViewType::HERO_STATIC, BodyType::STATIC_BODY, ViewType::STATIC_HERO);
		__RegisterBody(bodyData, ViewType::HERO_BULLET, BodyType::BULLET_BODY, ViewType::BULLET_HERO);
	}
	else if (type == BodyType::STATIC_BODY)
		__RegisterBody(bodyData, ViewType::STATIC_HERO, BodyType::PLAYER_BODY, ViewType::HERO_STATIC);
	else if (type == BodyType::BULLET_BODY)
		__RegisterBody(bodyData, ViewType::BULLET_HERO, BodyType::PLAYER_BODY, ViewType::HERO_BULLET);
	return true;
}
void DistanceProxy::UnregisterBody(ActorID id)
{
	UnregisterBody(DistanceObjMap[id]);//��ʼ��ע�������������
}

void DistanceProxy::UnregisterBody(BodyData* body)
{
	if (body->Type() == BodyType::MONSTER_BODY) {
		__UnregisterBody(body, ViewType::MONSTER_HERO, ViewType::HERO_MONSTER);
	}
	else if (body->Type() == BodyType::PLAYER_BODY)
	{
		__UnregisterBody(body, ViewType::HERO_MONSTER, ViewType::MONSTER_HERO);
		__UnregisterBody(body, ViewType::HERO_HERO, ViewType::VIEW_TYPE_NULL);
		__UnregisterBody(body, ViewType::HERO_STATIC, ViewType::STATIC_HERO);
		__UnregisterBody(body, ViewType::HERO_BULLET, ViewType::BULLET_HERO);
	}
	else if (body->Type() == BodyType::STATIC_BODY)
	{
		__UnregisterBody(body, ViewType::STATIC_HERO, ViewType::HERO_STATIC);
	}
	else if (body->Type() == BodyType::BULLET_BODY)
	{
		__UnregisterBody(body, ViewType::BULLET_HERO, ViewType::HERO_BULLET);
	}
	IgnoreCalcMap.erase(body->ID());
	DistanceObjMap.erase(body->ID());
	DistanceCalcRemove(body);
}

//��ȡ����Ұ��Χ�� �����һ��BodyData
b2Body* DistanceProxy::MinumumDistanceBody(ActorID checkID, ViewType checkType)
{
	BodyData* bodyData = DistanceObjMap[checkID];
	auto visibleList = DistanceMap[checkType][checkID][ViewStatus::VISIBLE];
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
void DistanceProxy::RecountDistance(BodyData* body) {
	if (body->Type() == BodyType::MONSTER_BODY) {
		__RecountDistance(body, ViewType::MONSTER_HERO, ViewType::HERO_MONSTER);
	}
	else if (body->Type() == BodyType::PLAYER_BODY) {
		__RecountDistance(body, ViewType::HERO_MONSTER, ViewType::MONSTER_HERO);
		__RecountDistance(body, ViewType::HERO_HERO, ViewType::VIEW_TYPE_NULL);
		__RecountDistance(body, ViewType::HERO_STATIC, ViewType::VIEW_TYPE_NULL);
		__RecountDistance(body, ViewType::HERO_BULLET, ViewType::BULLET_HERO);
	}
	else if (body->Type() == BodyType::BULLET_BODY) {
		__RecountDistance(body, ViewType::BULLET_HERO, ViewType::HERO_BULLET);
	}
}

//���¼������
void DistanceProxy::__RecountDistance(BodyData* body, ViewType viewType, ViewType addType)
{
	auto& inviewMap = DistanceMap[viewType][body->ID()][ViewStatus::INVISIBLE];//Ϊ�˷������ ��ȡ�������ӱ�
	auto& viewMap = DistanceMap[viewType][body->ID()][ViewStatus::VISIBLE];//Ϊ�˷������ ��ȡ�����ӱ�
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
	if (addType == ViewType::VIEW_TYPE_NULL)//��ִ��֮��Ĳ���
		goto con;
	{
		auto& visbleMainMap = DistanceMap[viewType][body->ID()][ViewStatus::VISIBLE];//��ȡ�����������������������Ϣ
		auto& typeMap = DistanceMap[addType];//��ȡ�� ���������Map
		for (auto item = typeMap.begin(); item != typeMap.end(); item++) {//��������ı���и���
			ViewStatus isMainVisble = visbleMainMap.find(item->first) != visbleMainMap.end() ? ViewStatus::VISIBLE : ViewStatus::INVISIBLE;//�ж����Ƿ��ڿɼ��б��У��ó��Ľ���ǵ�ǰ���ڵ��б�
			ViewStatus isInverterVisble = isMainVisble == ViewStatus::VISIBLE ? ViewStatus::INVISIBLE : ViewStatus::VISIBLE;//�ж����Ƿ��ڿɼ��б��У��ó��Ľ���ǵ�ǰ���ڵ��б�
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
void DistanceProxy::__RegisterBody(BodyData* body, ViewType viewType, BodyType listType, ViewType addType)
{
	const b2AABB& BodyAABB = body->GetViewAABB(); //���Ȼ�ȡ���������Ұ��Χ
	DistanceMap[viewType][body->ID()] = unordered_map< ViewStatus, unordered_map<ActorID, BodyData*>>();//���丳ֵ
	auto BodyList = BodyManager::Instance().GetBodyList(listType);//��ȡ������������б� 
	for (auto begin = BodyList.begin(); begin != BodyList.end(); begin++) {//��ʼ��������б�
		ActorID ID = begin->second->ID();
		BodyData* calcBody = DistanceObjMap[ID];
		ViewStatus viewStatus = b2TestOverlap(BodyAABB, calcBody->GetAABB()) ? ViewStatus::VISIBLE : ViewStatus::INVISIBLE;//����Ƿ�����ڹ�����Ұ��Χ��
		DistanceMap[viewType][body->ID()][viewStatus][begin->first] = calcBody; //�������ӵ���Ӧ��Ұ��Χ
	}
	if (addType == ViewType::VIEW_TYPE_NULL)//�Ƿ�������������
		goto con;
	{
		auto& visbleMainMap = DistanceMap[viewType][body->ID()][ViewStatus::VISIBLE];//��ȡ�����������������������Ϣ
		auto& typeMap = DistanceMap[addType];//��ȡ�� ���������Map
		for (auto item = typeMap.begin(); item != typeMap.end(); item++) {//��������ı���и���
			ViewStatus isMainVisble = visbleMainMap.find(item->first) != visbleMainMap.end() ? ViewStatus::VISIBLE : ViewStatus::INVISIBLE;//�ж����Ƿ��ڿɼ��б��У��ó��Ľ���ǵ�ǰ���ڵ��б�
			ViewStatus isInverterVisble = isMainVisble == ViewStatus::VISIBLE ? ViewStatus::INVISIBLE : ViewStatus::VISIBLE;//�ж����Ƿ��ڿɼ��б��У��ó��Ľ���ǵ�ǰ���ڵ��б�
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
void DistanceProxy::__UnregisterBody(BodyData* body, ViewType viewType, ViewType addType)
{
	auto viewMap = DistanceMap[viewType][body->ID()];
	auto visibleMap = viewMap[ViewStatus::VISIBLE];
	auto invisibleMap = viewMap[ViewStatus::INVISIBLE];
	if (addType != ViewType::VIEW_TYPE_NULL)//�Ƿ������������� 
	{
		//�������Կ����ĳ�Ա
		for (auto begin = visibleMap.begin(); begin != visibleMap.end(); begin++)
			DistanceMap[addType][begin->first][ViewStatus::VISIBLE].erase(body->ID());
		//���������Կ����ĳ�Ա
		for (auto begin = visibleMap.begin(); begin != visibleMap.end(); begin++)
			DistanceMap[addType][begin->first][ViewStatus::INVISIBLE].erase(body->ID());
	}
	DistanceMap[viewType].erase(body->ID());
}
void DistanceProxy::DistanceCalc() {
	if (!DelayedCalcMap.size())
		return;
	for (auto item = DelayedCalcMap.begin(); item != DelayedCalcMap.end(); item++)
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


void DistanceProxy::DistanceDump(ActorID id) {
	BodyData* bodyData = DistanceObjMap[id];
	if (bodyData->Type() == BodyType::MONSTER_BODY)
	{
		__Dump(id, ViewType::MONSTER_HERO);
	}
	else if (bodyData->Type() == BodyType::PLAYER_BODY)
	{
		__Dump(id, ViewType::HERO_HERO);
		__Dump(id, ViewType::HERO_MONSTER);
	}
}
//��ӡ��ǰ�ɼ�����
void DistanceProxy::__Dump(ActorID id, ViewType type)
{
	auto visbleMap = DistanceMap[type][id][ViewStatus::VISIBLE];
	auto invisbleMap = DistanceMap[type][id][ViewStatus::INVISIBLE];
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
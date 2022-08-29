#pragma once
#include "Manager/Base/BodyData.h" 
#include <unordered_map> 
#include <unordered_set>  
class BodyManager
{
private:
	b2World* World;//ÿ��������������һ�������ָ��,������ǰ���ɱ�������й���
private:
	BodyManager();//��ǰ�Ĺ��캯��
public:
	std::unordered_map<ActorID, BodyData*> BodyMap;//���ڴ洢����� 
	std::unordered_map<BodyType, std::unordered_map<ActorID, BodyData*>> BodyManagerMap;//ĳһ���͵�body����
public:
	void SetWorld(b2World* world);//���õ�ǰ���������,֮�����еĸ��嶼����ӵ�����
	bool RegisterBody(BodyType type, b2Body* body);//ע�ᵱǰ��body����
	const std::unordered_map<ActorID, BodyData*>& GetBodyList(BodyType type);//��ȡ����ǰ�ĸ�����Ϣ�б�
	inline b2Body* GetBody(ActorID id);//��ȡ��һ������
	inline BodyData* GetBodyData(ActorID id);
	BodyType GetType(ActorID id);//��ȡ��һ������
	b2Body* CreateBody(BodyType type, std::string name,b2Vec2 postion = b2Vec2(0,0));//����һ������
	void DeleteBody(b2Body* body);//ɾ��һ������
	void DeleteBody(ActorID ID);//ɾ��һ������
	static BodyManager& Instance();//��ȡ������
};
inline BodyData* BodyManager::GetBodyData(ActorID id)
{
	if (BodyMap.count(id) == 0)
		return NULL;
	return BodyMap[id];
}
inline b2Body* BodyManager::GetBody (ActorID id)
{
	if (BodyMap.count(id) == 0 )
		return NULL;
	return BodyMap[id]->GetBody();
}
inline BodyType BodyManager::GetType(ActorID id)
{
	if (BodyMap.count(id) == 0)
		return BodyType::BODY_TYPE_NULL;
	return BodyMap[id]->Type();
}
inline void BodyManager::SetWorld(b2World* world)
{
	World = world;
}
inline BodyManager& BodyManager::Instance() {
	static BodyManager Instance;
	return Instance;
}

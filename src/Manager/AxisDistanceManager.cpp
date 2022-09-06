#include "Manager/AxisDistanceManager.h" 
using namespace std;    
ViewRange::ViewRange(BodyData* bodyData, AutomaticGenerator<b2AABB>* allocObj, b2Vec2 range) :
	m_ObserverRange(range),
	m_BodyPos(MAX_DISTANCE, MAX_DISTANCE),
	m_Actor(bodyData),
	m_AABBAllocObj(allocObj) {}


void ViewRange::RecalcAABBRange(PointType type)//���¼����ӿ�
{
	b2AABB& aabbGrid = m_BodyAABB[type];
	if (type == PointType::VIEW_TYPE) { 
		aabbGrid.lowerBound.Set(m_BodyPos.x - m_ObserverRange.x, m_BodyPos.y - m_ObserverRange.x);//���¼�����ͼ
		aabbGrid.upperBound.Set(m_BodyPos.x + m_ObserverRange.y, m_BodyPos.y + m_ObserverRange.y);
	} else if(type == PointType::BODY_TYPE) {
		const b2AABB& bodyAABB = m_Actor->GetAABB();//��ȡ����ɫ��AABB
		memcpy(&m_BodyAABB[type], &bodyAABB, sizeof(b2AABB));//��������
		RecalcRefreshCondtion();
	}
}  

void ViewRange::CalcSplitAABB(PointType type)//����ָ����(AABB�ᾭ����,��ͼ��������)
{
	b2AABB& aabbGrid = m_BodyAABB[type];//AABB��Ԫ 
	float length = aabbGrid.upperBound.x - aabbGrid.lowerBound.x;//���Ȼ�ȡ����ǰ�����AABB�ĳ���
	int count = ceil(length / MAX_OVERFLOW_RANGE);//������Ҫ������ٸ�
	if(!count) count = 1;//���ٷ���һ�� 
	int splitCount = count - m_SplitAABBList[type].size();//��ȡ����ǰ�ķָ����
	if (splitCount < 0) {//�黹����
		for (int i = 0; i > splitCount; i--) {
			auto obj = m_SplitAABBList[type].begin();
			m_AABBAllocObj->BackObj(*obj);
			m_SplitAABBList[type].erase(obj);
		}
	} else if (splitCount > 0) {
		for (int i = 0; i > splitCount; i--) 
			m_SplitAABBList[type].push_back(m_AABBAllocObj->RequireObj()); 
	}
	float calcX = m_BodyAABB[type].lowerBound.x;//����ʼ,����
	for (auto item = m_SplitAABBList[type].begin(); item != m_SplitAABBList[type].end(); item++){
		b2AABB* aabb = *item;
		aabb->lowerBound.Set(calcX, m_BodyAABB[type].lowerBound.y);
		if (calcX + MAX_OVERFLOW_RANGE < m_BodyAABB[type].upperBound.x)
			aabb->upperBound.Set(calcX + MAX_OVERFLOW_RANGE, m_BodyAABB[type].upperBound.y);
		else
			aabb->upperBound.Set(m_BodyAABB[type].upperBound.x, m_BodyAABB[type].upperBound.y);
	}
}  

//�Լ��ƶ��Ļ�
void ViewRange::MoveSelf() {//֪ͨÿ���۲쵽�Լ�����,��ǰ�Լ��ƶ���
	for (auto item = m_BeObserverMap.begin(); item != m_BeObserverMap.end(); ) {
		if (!item->second->IsContain(this))//����Լ���ǰ���ڶԷ�����Ұ��
			m_BeObserverMap.erase(item++);
		else
			item++;
	}
}

bool ViewRange::IsContain(ViewRange* actor) {//�ƶ��Ľ�ɫѯ�ʵ�ǰ�Ƿ��ڼ�����Χ��
	ActorID id = actor->m_Actor->ID();
	bool isObserver = this->GetBodyAABB(PointType::VIEW_TYPE).Contains(actor->GetBodyAABB(PointType::BODY_TYPE));//�ж�����AABB�Ƿ��ص�
	if (!isObserver && m_ObserverMap.count(id))//�����ǰ���ù۲쵽���������,�����ڿ�������
		m_ObserverMap.erase(id);
	return isObserver;//ɾ���Լ�
}

ViewRange::~ViewRange()
{
	RecycleAABBPoint(PointType::BODY_TYPE);
	RecycleAABBPoint(PointType::VIEW_TYPE);
}


AxisDistanceManager::AxisDistanceManager() {
}
//��ʹ�ñ�����֮ǰӦȷ��,��ǰ���б���,���޶�Ӧ��ɫ
inline void AxisDistanceManager::AdditionDistancePoint(ActorID actorID, PointType addType, const b2AABB& viewRange)
{
	ActorID minActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MIN); //������СID
	ActorID maxActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MAX); //�������ID 
	ViewRangeRecordMap& xRangeRecordMap = m_AxisBodyMap[AxisType::X_AXIS];
	ViewRangeRecordMap& yRangeRecordMap = m_AxisBodyMap[AxisType::Y_AXIS]; 
	if (!xRangeRecordMap.count(viewRange.lowerBound.x)) //�����ڵ�ǰ�����Ļ�   
		xRangeRecordMap[viewRange.lowerBound.x] = m_ViewCellIdleGenerate.RequireObj();
	if (!xRangeRecordMap.count(viewRange.upperBound.x)) //�����ڵ�ǰ�����Ļ�   
		xRangeRecordMap[viewRange.upperBound.x] = m_ViewCellIdleGenerate.RequireObj(); 
	if (!yRangeRecordMap.count(viewRange.lowerBound.y)) //�����ڵ�ǰ�����Ļ�   
		yRangeRecordMap[viewRange.lowerBound.y] = m_ViewCellIdleGenerate.RequireObj(); 
	if (!yRangeRecordMap.count(viewRange.upperBound.y)) //�����ڵ�ǰ�����Ļ�   
		yRangeRecordMap[viewRange.upperBound.y] = m_ViewCellIdleGenerate.RequireObj();
	(*xRangeRecordMap[viewRange.lowerBound.x])[addType].insert(minActorID);
	(*xRangeRecordMap[viewRange.upperBound.x])[addType].insert(maxActorID);
	(*yRangeRecordMap[viewRange.lowerBound.y])[addType].insert(minActorID);
	(*yRangeRecordMap[viewRange.upperBound.y])[addType].insert(maxActorID);
}
inline void AxisDistanceManager::RemoveDistancePoint(ActorID actorID, PointType type, const b2AABB& viewRange)
{
	ActorID minActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MIN); //������СID
	ActorID maxActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MAX); //�������ID 
	ViewRangeRecordMap& xRangeRecordMap = m_AxisBodyMap[AxisType::X_AXIS];//��ȡ��X�����Ϣ
	ViewRangeRecordMap& yRangeRecordMap = m_AxisBodyMap[AxisType::Y_AXIS];//��ȡ��Y�����Ϣ
	if (xRangeRecordMap.count(viewRange.lowerBound.x))//ɾ�����ID��Ӧ��X����� 
			(*xRangeRecordMap[viewRange.lowerBound.x])[type].erase(minActorID);
	if (xRangeRecordMap.count(viewRange.upperBound.x))//���ڵ�ǰ�����Ļ�  
			(*xRangeRecordMap[viewRange.upperBound.x])[type].erase(maxActorID);
	if (yRangeRecordMap.count(viewRange.lowerBound.y))//���ڵ�ǰ�����Ļ�  
			(*yRangeRecordMap[viewRange.lowerBound.y])[type].erase(minActorID);
	if (yRangeRecordMap.count(viewRange.upperBound.y))//���ڵ�ǰ�����Ļ�  
			(*yRangeRecordMap[viewRange.upperBound.y])[type].erase(maxActorID);
	ABBBRnageRecycle(viewRange.lowerBound.x, xRangeRecordMap);
	ABBBRnageRecycle(viewRange.upperBound.x, xRangeRecordMap);
	ABBBRnageRecycle(viewRange.lowerBound.y, yRangeRecordMap);
	ABBBRnageRecycle(viewRange.upperBound.y, yRangeRecordMap);
}

void AxisDistanceManager::ActorsMove()
{ 
	ViewRange* actorView = NULL;//�����ƶ����еĽ�ɫ
	bool OnlyCalculate = false;//��ǰ�Ƿ��������
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end();) {
		ActorID actorID = *item;
		OnlyCalculate = false;
		if (!m_ViewObjMap.count(actorID))//���󲻴���,ֱ��ɾ��
			goto erase;
		actorView = m_ViewObjMap[actorID];
		if (actorView->IsRefreshActorView() == 0 )//�ƶ������ǳ
			goto calc_erase;//���¼���λ��,����ȴ
		actorView->RecalcBodyPosition();//�ؼ����ɫ��λ��
		OnlyCalculate = true;
	calc_erase:
		RemoveDistanceAABB(actorView, PointType::BODY_TYPE);//ɾ��body
		RemoveDistanceAABB(actorView, PointType::VIEW_TYPE);//ɾ��view 
		actorView->RecalcBodyAABB(PointType::BODY_TYPE);//���¼��㵱ǰ����ײ����
		actorView->RecalcBodyAABB(PointType::VIEW_TYPE);//���¼��㵱ǰ����ͼ���� 
		AdditionDistanceAABB(actorView, PointType::BODY_TYPE);//�����е������ȥ
		AdditionDistanceAABB(actorView, PointType::VIEW_TYPE);//�����е������ȥ 
		item++;
		if (OnlyCalculate) continue;
	erase:
		m_DelayCalcMoveList.erase(item++);
	}
}
//Ѱ��ĳһ�����ĳһ��Χ�ڵ����е�Ԫ��Ϣ
bool AxisDistanceManager::InquiryAxisPoints(unordered_set<ActorID>& outData, float findKey, PointType type, AxisType axis, float inquiryDistance, float leftOffset, float rightOffset)//ƫ�Ƶ�Ԫ����ǰҪ��ѯ��Ϣ(����ȷ����ǰ����ѯ������ڱ���)
{
	if (!m_AxisBodyMap[axis].count(findKey)) return false;
	auto axisMap = m_AxisBodyMap[axis];
	auto beginIterator = axisMap.find(findKey);//����
	std::map<float, ViewRangeTypeSet*>::reverse_iterator rBeginITerator(beginIterator);//����
	float rightDistance = inquiryDistance + rightOffset;//��ѯ�����վ���
	for (auto item = beginIterator; item != axisMap.end() && item->first <= rightDistance; item++) {
		auto typeMap = (*item->second)[type];
		for (auto distanceItem = typeMap.begin(); distanceItem != typeMap.end(); distanceItem++)
			outData.insert(GET_ACTOR_ID(*distanceItem));
	}
	//���Ȼ�ȡ����ǰѰ�ҵ���λ��
	float leftDistance = inquiryDistance - leftOffset;
	if (leftOffset >= inquiryDistance) goto out; 
	rBeginITerator++;//֮ǰ�ļ�����,�Ѿ��������Լ�
	for (auto item = rBeginITerator; item != axisMap.rend() && item->first >= leftDistance; item++) {
		auto typeMap = (*item->second)[type];
		for (auto distanceItem = typeMap.begin(); distanceItem != typeMap.end(); distanceItem++)
			outData.insert(GET_ACTOR_ID(*distanceItem));
	}
out:
	return true;
}
//�ؼ���
inline void AxisDistanceManager::CalcViewObj()
{
	//ÿ����ɫ���ƶ�һ���Լ�,ˢ�µ�ǰ�ı�ˢ�¶���
	for (auto actorIDItem = m_DelayCalcMoveList.begin(); actorIDItem != m_DelayCalcMoveList.end();actorIDItem++) {  
		ViewRange* actoViewange = m_ViewObjMap[*actorIDItem];
		actoViewange->MoveSelf();//�����Լ��ƶ���
	}

	//���¼����Լ���������
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end();) {
		//���ȼ��㵱ǰ�Ĳ��� 
		ActorID actorID = *item;
		ViewRange* actoViewange = m_ViewObjMap[actorID];
		actoViewange->ClearObserverTable();//���,Ȼ�����¼���
		unordered_set<ActorID> XSet;
		const b2AABB& viewAABB = actoViewange->GetBodyAABB(PointType::VIEW_TYPE);
		InquiryAxisPoints(XSet, viewAABB.lowerBound.x, PointType::BODY_TYPE, AxisType::X_AXIS, viewAABB.upperBound.x - viewAABB.lowerBound.x, 0,MAX_OVERFLOW_RANGE);//��ȡ��X����б�
		InquiryAxisPoints(XSet, viewAABB.lowerBound.y, PointType::BODY_TYPE, AxisType::Y_AXIS, viewAABB.upperBound.y - viewAABB.lowerBound.y, 0, MAX_OVERFLOW_RANGE);//��ȡ��X����б� 
		//���й����Ľ�ɫͳͳ����һ�¾���

		for (auto relevanceItem = XSet.begin(); relevanceItem != XSet.end(); relevanceItem++) {
			auto obseverItem = m_ViewObjMap[*relevanceItem];
			//����AABB
			if (obseverItem->GetBodyAABB(PointType::BODY_TYPE).Contains(actoViewange->GetBodyAABB(PointType::VIEW_TYPE)))
			{
				actoViewange->EnterView(obseverItem);
			}
		}
	}
}
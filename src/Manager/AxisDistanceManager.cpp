#include "Manager/AxisDistanceManager.h" 
using namespace std;    
ViewRange::ViewRange(BodyData* bodyData, b2Vec2 range) :
	m_ObserverRange(range),
	m_BodyPos(MAX_DISTANCE, MAX_DISTANCE),
	m_Actor(bodyData), 
	m_OffsetCondtion(-9999,-9999){
} 

//�Լ��ƶ��Ļ�
void ViewRange::MoveSelf() {//֪ͨÿ���۲쵽�Լ�����,��ǰ�Լ��ƶ���
	for (auto item = m_BeObserverMap.begin(); item != m_BeObserverMap.end(); ) {
		if (!item->second->InObserverContain(this))//����Լ���ǰ���ڶԷ�����Ұ��
			m_BeObserverMap.erase(item++);
		else
			item++;
	}
} 
 
void AxisDistanceManager::ActorsMove()
{
	ViewRange* actorView = NULL;//�����ƶ����еĽ�ɫ
	bool isDelete = false;//��ǰ�Ƿ��������  
	auto iterEnd = m_DelayCalcMoveList.end();
	for (auto item = m_DelayCalcMoveList.begin(); item != iterEnd;item++) { 
		ActorID actorID = item->first; 
		actorView = item->second;
		if (actorView->IsRefreshActorView() == 0) {//�ƶ������ǳ
			continue;
		}
		actorView->RecalcBodyPosition();//�ؼ����ɫ��λ��  
		RemoveAABB(PointType::BODY_TYPE, actorView);
		RemoveAABB(PointType::VIEW_TYPE, actorView); 
		actorView->RecalcBodyAABB();
		actorView->RecalcViewAABB();
		AdditionAABB(PointType::BODY_TYPE, actorView);
		AdditionAABB(PointType::VIEW_TYPE, actorView);
		m_CalcMoveList[actorID] = item->second;
	}  
	m_DelayCalcMoveList.clear(); 
}



void AxisDistanceManager::AdditionAABB(PointType type, ViewRange* actoViewRange)
{
	const b2AABB& viewAABB = actoViewRange->GetAABB(type);//��ȡ����Ӧ���͵�AABB
	b2Vec2 leftMax(viewAABB.lowerBound.x, viewAABB.upperBound.y);//POS_BODY_LIMIT_LEFT_MAX
	b2Vec2 leftMin(viewAABB.lowerBound.x, viewAABB.lowerBound.y);//POS_BODY_LIMIT_LEFT_MIN
	b2Vec2 rightMax(viewAABB.upperBound.x, viewAABB.upperBound.y);//POS_BODY_LIMIT_RIGHT_MAX
	b2Vec2 rightMin(viewAABB.upperBound.x, viewAABB.lowerBound.y); //POS_BODY_LIMIT_RIGHT_MIN  
	b2Vec2* posPoint[PointPosType::POS_BODY_LIMIT_NUM_MAX] = { &leftMax,&leftMin,&rightMax,&rightMin };
	for (int i = 0; i < PointPosType::POS_BODY_LIMIT_NUM_MAX; i++) {
		if (!m_AxisBodyMap[type].count(*posPoint[i])) {
			m_AxisBodyMap[type][*posPoint[i]] = m_ViewCellIdleGenerate.RequireObj();
		}
		m_AxisBodyMap[type][*posPoint[i]]->Insert(actoViewRange->GetActor()->ID(), (PointPosType)i);
	}
}
void AxisDistanceManager::RemoveAABB(PointType type, ViewRange* actoViewRange)
{
	const b2AABB& viewAABB = actoViewRange->GetAABB(type);//��ȡ����Ӧ���͵�AABB
	b2Vec2 leftMax(viewAABB.lowerBound.x, viewAABB.upperBound.y);//POS_BODY_LIMIT_LEFT_MAX
	b2Vec2 leftMin(viewAABB.lowerBound.x, viewAABB.lowerBound.y);//POS_BODY_LIMIT_LEFT_MIN
	b2Vec2 rightMax(viewAABB.upperBound.x, viewAABB.upperBound.y);//POS_BODY_LIMIT_RIGHT_MAX
	b2Vec2 rightMin(viewAABB.upperBound.x, viewAABB.lowerBound.y); //POS_BODY_LIMIT_RIGHT_MIN  
	b2Vec2* posPoint[PointPosType::POS_BODY_LIMIT_NUM_MAX] = { &leftMax,&leftMin,&rightMax,&rightMin };
	ViewRangeRecordMap& axisCell = m_AxisBodyMap[type];
	ActorID actorID = actoViewRange->GetActor()->ID();
	for (int i = 0; i < PointPosType::POS_BODY_LIMIT_NUM_MAX; i++) {
		b2Vec2& pos = *posPoint[i];
		if (!axisCell.count(*posPoint[i]))//�����ж϶�Ӧ�����Ƿ��е�ǰ��Ϣ
			continue;
		axisCell[*posPoint[i]]->Erease(actorID, (PointPosType)i);//ɾ�������Ԫ
		if (axisCell[*posPoint[i]]->IsInValid()) {
			m_ViewCellIdleGenerate.BackObj(axisCell[*posPoint[i]]);
			axisCell.erase(*posPoint[i]);
		}
	}
}

extern ActorID playerID;
//Ѱ��ĳһ�����ĳһ��Χ�ڵ����е�Ԫ��Ϣ  
void AxisDistanceManager::InquiryAxisPoints(unordered_set<ActorID>& outData,const b2AABB& viewAABB)//ƫ�Ƶ�Ԫ����ǰҪ��ѯ��Ϣ(����ȷ����ǰ����ѯ������ڱ���)
{ 
	b2Vec2 tempPoint = viewAABB.lowerBound;//��ȡ����ǰ����ͼ��͵�
	ViewRangeRecordMap::iterator iterEnd = m_AxisBodyMap[PointType::BODY_TYPE].end();//��ȡ��С�ڵ��ڵ�ǰֵ�Ķ���  
	ViewRangeRecordMap::iterator iter = m_AxisBodyMap[PointType::BODY_TYPE].lower_bound(tempPoint);
	while (iter != iterEnd && iter->first.x <= viewAABB.upperBound.x)//ĩβ ���� Խ��
	{	
		while (iter != iterEnd && (iter->first.y > viewAABB.upperBound.y || iter->first.y < viewAABB.lowerBound.y))//�������Χ�ڵĻ�,�Ͳ�ͣ��Ѱ����һ��
		{
			if (iter->first.y > viewAABB.upperBound.y) {
				tempPoint.Set(iter->first.x, 99999);
				iter = m_AxisBodyMap[PointType::BODY_TYPE].upper_bound(tempPoint);//����x��
			} 
			if (iter != iterEnd && iter->first.y < viewAABB.lowerBound.y) {
				tempPoint.Set(iter->first.x, viewAABB.lowerBound.y); 
				iter = m_AxisBodyMap[PointType::BODY_TYPE].lower_bound(tempPoint);//����x��
			}
		}
		if (iter == iterEnd) break;
		for (auto item = iter->second->GetTable().begin(); item != iter->second->GetTable().end(); item++) {
			ActorID id = GET_ACTOR_ID(*item);
			if (outData.count(id))
				continue;
			outData.insert(id); 
		}
		iter++;
	} 
}
//�ؼ���
void AxisDistanceManager::CalcViewObj()
{
	//ÿ����ɫ���ƶ�һ���Լ�,ˢ�µ�ǰ�ı�ˢ�¶���
	for (auto actorItem = m_CalcMoveList.begin(); actorItem != m_CalcMoveList.end(); actorItem++)
		actorItem->second->MoveSelf();//�����Լ��ƶ���

	//���¼����Լ���������
	ViewRange* actoViewange = NULL;
	for (auto item = m_CalcMoveList.begin(); item != m_CalcMoveList.end(); item++) {
		//���ȼ��㵱ǰ�Ĳ��� 
		ActorID actorID = item->first; 
		actoViewange = item->second;
		actoViewange->ClearObserverTable();//���,Ȼ�����¼���
		unordered_set<ActorID> XSet;
		const b2AABB& viewAABB = actoViewange->GetViewAABB();
		InquiryAxisPoints(XSet, viewAABB);//��ȡ��X����б�  
		for (auto relevanceItem = XSet.begin(); relevanceItem != XSet.end(); relevanceItem++) {
			auto obseverItem = m_ViewObjMap[*relevanceItem]; 
			actoViewange->EnterView(obseverItem); 
		}
	}
	m_CalcMoveList.clear();
} 
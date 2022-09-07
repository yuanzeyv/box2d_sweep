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
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end();) {
		ActorID actorID = item->first; 
		actorView = item->second;
		if (actorView->IsRefreshActorView() == 0)//�ƶ������ǳ
			goto erase;//���¼���λ��,����ȴ
		actorView->RecalcBodyPosition();//�ؼ����ɫ��λ�� 
	calc_erase:
		RemoveAABB(PointType::BODY_TYPE, actorView);
		RemoveAABB(PointType::VIEW_TYPE, actorView);
		actorView->RecalcBodyAABB();
		actorView->RecalcViewAABB();
		AdditionAABB(PointType::BODY_TYPE, actorView);
		AdditionAABB(PointType::VIEW_TYPE, actorView); 
		item++;
		continue; 
	erase:
		m_DelayCalcMoveList.erase(item++);
	}  
}


void AxisDistanceManager::AdditionAABB(PointType type, ViewRange* actoViewRange)
{
	const b2AABB& viewAABB = actoViewRange->GetAABB(type);//��ȡ����ǰ���͵�AABB
	//��������ĸ���
	b2Vec2 posPoint[PointPosType::POS_BODY_LIMIT_NUM_MAX] = {
		b2Vec2(viewAABB.lowerBound.x, viewAABB.upperBound.y),//POS_BODY_LIMIT_LEFT_MAX
		b2Vec2(viewAABB.lowerBound),//POS_BODY_LIMIT_LEFT_MIN
		b2Vec2(viewAABB.upperBound),//POS_BODY_LIMIT_RIGHT_MAX
		b2Vec2(viewAABB.upperBound.x, viewAABB.lowerBound.y)//POS_BODY_LIMIT_RIGHT_MIN
	};

	for (int i = 0; i < PointPosType::POS_BODY_LIMIT_NUM_MAX; i++) {
		if (!m_AxisBodyMap[type].count(posPoint[i])) {
			m_AxisBodyMap[type][posPoint[i]] = m_ViewCellIdleGenerate.RequireObj();
		}
		m_AxisBodyMap[type][posPoint[i]]->Insert(actoViewRange->GetActor()->ID(), (PointPosType)i);
	} 
}
void AxisDistanceManager::RemoveAABB(PointType type, ViewRange* actoViewRange)
{
	const b2AABB& viewAABB = actoViewRange->GetAABB(type);//��ȡ����Ӧ���͵�AABB
	b2Vec2 posPoint[PointPosType::POS_BODY_LIMIT_NUM_MAX] = {
		b2Vec2(viewAABB.lowerBound.x, viewAABB.upperBound.y),//POS_BODY_LIMIT_LEFT_MAX
		b2Vec2(viewAABB.lowerBound),//POS_BODY_LIMIT_LEFT_MIN
		b2Vec2(viewAABB.upperBound),//POS_BODY_LIMIT_RIGHT_MAX
		b2Vec2(viewAABB.upperBound.x, viewAABB.lowerBound.y) }; //POS_BODY_LIMIT_RIGHT_MIN

	for (int i = 0; i < PointPosType::POS_BODY_LIMIT_NUM_MAX; i++) {
		if (!m_AxisBodyMap[type].count(posPoint[i]))//�����ж϶�Ӧ�����Ƿ��е�ǰ��Ϣ
			continue;
		m_AxisBodyMap[type][posPoint[i]]->Erease(actoViewRange->GetActor()->ID(), (PointPosType)i);//ɾ�������Ԫ
		if (m_AxisBodyMap[type][posPoint[i]]->IsInValid()) {
			m_ViewCellIdleGenerate.BackObj(m_AxisBodyMap[type][posPoint[i]]);
			m_AxisBodyMap[type].erase(posPoint[i]);
		}
	}
}
//Ѱ��ĳһ�����ĳһ��Χ�ڵ����е�Ԫ��Ϣ
void AxisDistanceManager::InquiryAxisPoints(unordered_set<ActorID>& outData,const b2AABB& viewAABB)//ƫ�Ƶ�Ԫ����ǰҪ��ѯ��Ϣ(����ȷ����ǰ����ѯ������ڱ���)
{
	b2Vec2 tempPoint = viewAABB.lowerBound;//��ȡ����ǰ����ͼ��͵�
	auto iter = m_AxisBodyMap[PointType::BODY_TYPE].lower_bound(tempPoint);//��ȡ��С�ڵ��ڵ�ǰֵ�Ķ���
	auto iterEnd = m_AxisBodyMap[PointType::BODY_TYPE].end();//��ȡ��С�ڵ��ڵ�ǰֵ�Ķ���

	while (iter != m_AxisBodyMap[PointType::BODY_TYPE].end() && iter->first.x <= viewAABB.upperBound.x)
	{
		while (1) {
			for (auto item = iter->second->GetTable().begin(); item != iter->second->GetTable().end(); item++)
				outData.insert(GET_ACTOR_ID(*item));
			iter++;//Ѱ����һ��
			if (iter == iterEnd)
				return;
			if (iter->first.x > viewAABB.upperBound.x)//δԽ��Ļ�
				return;//Խ��ֱ�ӷ���
			if (iter->first.y < viewAABB.lowerBound.y || iter->first.y > viewAABB.upperBound.y)//�ڿ����̷�Χ֮�� 
				break;
		}
		tempPoint.Set(iter->first.x, 99999);//���¸�ֵ
		iter = m_AxisBodyMap[PointType::BODY_TYPE].upper_bound(tempPoint);
	}  
}
//�ؼ���
void AxisDistanceManager::CalcViewObj()
{
	//ÿ����ɫ���ƶ�һ���Լ�,ˢ�µ�ǰ�ı�ˢ�¶���
	for (auto actorItem = m_DelayCalcMoveList.begin(); actorItem != m_DelayCalcMoveList.end(); actorItem++)
		actorItem->second->MoveSelf();//�����Լ��ƶ���

	//���¼����Լ���������
	ViewRange* actoViewange = NULL;
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end(); item++) {
		//���ȼ��㵱ǰ�Ĳ��� 
		ActorID actorID = item->first; 
		actoViewange = item->second;
		actoViewange->ClearObserverTable();//���,Ȼ�����¼���
		unordered_set<ActorID> XSet;
		const b2AABB& viewAABB = actoViewange->GetBodyAABB();
		InquiryAxisPoints(XSet, viewAABB);//��ȡ��X����б�  
		for (auto relevanceItem = XSet.begin(); relevanceItem != XSet.end(); relevanceItem++) {
			auto obseverItem = m_ViewObjMap[*relevanceItem]; 
			actoViewange->EnterView(obseverItem); 
		}
	}
} 
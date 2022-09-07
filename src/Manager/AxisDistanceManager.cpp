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
	auto deleteSum = 0;
	auto recalcSum = 0;
	auto addSum = 0;
	ViewRange* actorView = NULL;//�����ƶ����еĽ�ɫ
	bool OnlyCalculate = false;//��ǰ�Ƿ��������
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end();) {
		ActorID actorID = *item;
		OnlyCalculate = false;
		if (!m_ViewObjMap.count(actorID))//���󲻴���,ֱ��ɾ��
			goto erase;
		actorView = m_ViewObjMap[actorID];
		if (actorView->IsRefreshActorView() == 0 )//�ƶ������ǳ
			goto erase;//���¼���λ��,����ȴ
		actorView->RecalcBodyPosition();//�ؼ����ɫ��λ��
		OnlyCalculate = true;
		calc_erase: 
		RemoveViewAABB(actorView);
		actorView->RecalcBodyAABB();
		actorView->RecalcViewAABB(); 
		AdditionViewAABB(actorView);
		if (OnlyCalculate) {
			item++;
			continue;
		}
	erase:
		m_DelayCalcMoveList.erase(item++);
	} 
	printf("ɾ����ʱ�� %lld  ������ʱ�� %lld  �����ʱ��%lld\n\r", deleteSum, recalcSum, addSum);
}


void AxisDistanceManager::AdditionViewAABB(ViewRange* actoViewRange)
{ 
	//��ȡ����ǰ��AABB��Ϣ
	const b2AABB& viewAABB = actoViewRange->GetViewAABB();
	//��������ĸ���
	b2Vec2 posPoint[PointPosType::POS_BODY_LIMIT_NUM_MAX];//�����ĸ�
	posPoint[PointPosType::POS_BODY_LIMIT_LEFT_MIN].Set(viewAABB.lowerBound);
	posPoint[PointPosType::POS_BODY_LIMIT_LEFT_MAX].Set(viewAABB.lowerBound.x, viewAABB.upperBound.y);
	posPoint[PointPosType::POS_BODY_LIMIT_RIGHT_MIN].Set(viewAABB.upperBound.x, viewAABB.lowerBound.y);
	posPoint[PointPosType::POS_BODY_LIMIT_RIGHT_MAX].Set(viewAABB.upperBound); 
	for (int i = 0; i < PointPosType::POS_BODY_LIMIT_NUM_MAX; i++) {
		if (!m_AxisBodyMap.count(posPoint[i])) {
			m_AxisBodyMap[posPoint[i]] = m_ViewCellIdleGenerate.RequireObj();
		}
		m_AxisBodyMap[posPoint[i]]->Insert(actoViewRange->GetActor()->ID(), (PointPosType)i);
	} 
}
//Ѱ��ĳһ�����ĳһ��Χ�ڵ����е�Ԫ��Ϣ
void AxisDistanceManager::InquiryAxisPoints(unordered_set<ActorID>& outData,const b2AABB& viewAABB)//ƫ�Ƶ�Ԫ����ǰҪ��ѯ��Ϣ(����ȷ����ǰ����ѯ������ڱ���)
{
	b2Vec2 tempPoint = viewAABB.lowerBound;
	auto iter = m_AxisBodyMap.lower_bound(tempPoint);//��ȡ��С�ڵ��ڵ�ǰֵ�Ķ���
	do {
		while (1) {
			for (auto item = iter->second->GetTable().begin();item != iter->second->GetTable().end();item++)
				outData.insert(GET_ACTOR_ID(*item));
			iter++;//Ѱ����һ��
			if (iter->first.x > viewAABB.upperBound.x)//δԽ��Ļ�
				return;//Խ��ֱ�ӷ���
			if (iter->first.y < viewAABB.lowerBound.y || iter->first.y > viewAABB.upperBound.y)//�ڿ����̷�Χ֮�� 
				break;
		}
		tempPoint.Set(iter->first.x, 99999);//���¸�ֵ
		iter = m_AxisBodyMap.upper_bound(tempPoint);
	} while (iter != m_AxisBodyMap.end() && iter->first.x <= viewAABB.upperBound.x);
}
void AxisDistanceManager::RemoveViewAABB(ViewRange* actoViewRange)
{
	//��ȡ����ǰ��AABB��Ϣ
	const b2AABB& viewAABB = actoViewRange->GetViewAABB();
	//��������ĸ���
	b2Vec2 posPoint[PointPosType::POS_BODY_LIMIT_NUM_MAX];//�����ĸ�
	posPoint[PointPosType::POS_BODY_LIMIT_LEFT_MIN].Set(viewAABB.lowerBound);
	posPoint[PointPosType::POS_BODY_LIMIT_LEFT_MAX].Set(viewAABB.lowerBound.x, viewAABB.upperBound.y);
	posPoint[PointPosType::POS_BODY_LIMIT_RIGHT_MIN].Set(viewAABB.upperBound.x, viewAABB.lowerBound.y);
	posPoint[PointPosType::POS_BODY_LIMIT_RIGHT_MAX].Set(viewAABB.upperBound);
	for (int i = 0; i < PointPosType::POS_BODY_LIMIT_NUM_MAX; i++) {
		if (!m_AxisBodyMap.count(posPoint[i])) 
			continue; 
		m_AxisBodyMap[posPoint[i]]->Erease(actoViewRange->GetActor()->ID(), (PointPosType)i);
		if (m_AxisBodyMap[posPoint[i]]->IsInValid()){
			m_ViewCellIdleGenerate.BackObj(m_AxisBodyMap[posPoint[i]]);
			m_AxisBodyMap.erase(posPoint[i]);
		}
	}
}
//�ؼ���
void AxisDistanceManager::CalcViewObj()
{
	//ÿ����ɫ���ƶ�һ���Լ�,ˢ�µ�ǰ�ı�ˢ�¶���
	for (auto actorIDItem = m_DelayCalcMoveList.begin(); actorIDItem != m_DelayCalcMoveList.end();actorIDItem++) {  
		ViewRange* actoViewange = m_ViewObjMap[*actorIDItem];
		actoViewange->MoveSelf();//�����Լ��ƶ���
	}

	//���¼����Լ���������
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end(); item++) {
		//���ȼ��㵱ǰ�Ĳ��� 
		ActorID actorID = *item; 
		ViewRange* actoViewange = m_ViewObjMap[actorID];
		actoViewange->ClearObserverTable();//���,Ȼ�����¼���
		unordered_set<ActorID> XSet;
		const b2AABB& viewAABB = actoViewange->GetBodyAABB();
		InquiryAxisPoints(XSet, viewAABB);//��ȡ��X����б� 
		//���й����Ľ�ɫͳͳ����һ�¾���

		for (auto relevanceItem = XSet.begin(); relevanceItem != XSet.end(); relevanceItem++) {
			auto obseverItem = m_ViewObjMap[*relevanceItem]; 
			actoViewange->EnterView(obseverItem); 
		}
	}
} 
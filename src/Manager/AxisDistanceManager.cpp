#include "Manager/AxisDistanceManager.h" 
using namespace std;    
ViewRange::ViewRange(BodyData* bodyData, b2Vec2 range) :
	m_ObserverRange(range),
	m_BodyPos(MAX_DISTANCE, MAX_DISTANCE),
	m_Actor(bodyData), 
	m_OffsetCondtion(-9999,-9999){
} 

//自己移动的话
void ViewRange::MoveSelf() {//通知每个观察到自己的人,当前自己移动了
	for (auto item = m_BeObserverMap.begin(); item != m_BeObserverMap.end(); ) {
		if (!item->second->InObserverContain(this))//如果自己当前还在对方的视野下
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
	ViewRange* actorView = NULL;//首先移动所有的角色
	bool OnlyCalculate = false;//当前是否仅仅计算
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end();) {
		ActorID actorID = *item;
		OnlyCalculate = false;
		if (!m_ViewObjMap.count(actorID))//对象不存在,直接删除
			goto erase;
		actorView = m_ViewObjMap[actorID];
		if (actorView->IsRefreshActorView() == 0 )//移动距离过浅
			goto erase;//重新计算位置,但是却
		actorView->RecalcBodyPosition();//重计算角色的位置
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
	printf("删除总时长 %lld  计算总时长 %lld  添加总时长%lld\n\r", deleteSum, recalcSum, addSum);
}


void AxisDistanceManager::AdditionViewAABB(ViewRange* actoViewRange)
{ 
	//获取到当前的AABB信息
	const b2AABB& viewAABB = actoViewRange->GetViewAABB();
	//待加入的四个点
	b2Vec2 posPoint[PointPosType::POS_BODY_LIMIT_NUM_MAX];//分配四个
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
//寻找某一个轴的某一范围内的所有单元信息
void AxisDistanceManager::InquiryAxisPoints(unordered_set<ActorID>& outData,const b2AABB& viewAABB)//偏移单元代表当前要查询的息(必须确保当前被查询点存在于表内)
{
	b2Vec2 tempPoint = viewAABB.lowerBound;
	auto iter = m_AxisBodyMap.lower_bound(tempPoint);//获取到小于等于当前值的东西
	do {
		while (1) {
			for (auto item = iter->second->GetTable().begin();item != iter->second->GetTable().end();item++)
				outData.insert(GET_ACTOR_ID(*item));
			iter++;//寻找下一个
			if (iter->first.x > viewAABB.upperBound.x)//未越界的话
				return;//越界直接返回
			if (iter->first.y < viewAABB.lowerBound.y || iter->first.y > viewAABB.upperBound.y)//在可容忍范围之间 
				break;
		}
		tempPoint.Set(iter->first.x, 99999);//重新赋值
		iter = m_AxisBodyMap.upper_bound(tempPoint);
	} while (iter != m_AxisBodyMap.end() && iter->first.x <= viewAABB.upperBound.x);
}
void AxisDistanceManager::RemoveViewAABB(ViewRange* actoViewRange)
{
	//获取到当前的AABB信息
	const b2AABB& viewAABB = actoViewRange->GetViewAABB();
	//待加入的四个点
	b2Vec2 posPoint[PointPosType::POS_BODY_LIMIT_NUM_MAX];//分配四个
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
//重计算
void AxisDistanceManager::CalcViewObj()
{
	//每个角色先移动一下自己,刷新当前的被刷新对象
	for (auto actorIDItem = m_DelayCalcMoveList.begin(); actorIDItem != m_DelayCalcMoveList.end();actorIDItem++) {  
		ViewRange* actoViewange = m_ViewObjMap[*actorIDItem];
		actoViewange->MoveSelf();//代表自己移动了
	}

	//重新计算自己所见所得
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end(); item++) {
		//首先计算当前的操作 
		ActorID actorID = *item; 
		ViewRange* actoViewange = m_ViewObjMap[actorID];
		actoViewange->ClearObserverTable();//清除,然后重新计算
		unordered_set<ActorID> XSet;
		const b2AABB& viewAABB = actoViewange->GetBodyAABB();
		InquiryAxisPoints(XSet, viewAABB);//获取到X轴的列表 
		//所有关联的角色统统计算一下距离

		for (auto relevanceItem = XSet.begin(); relevanceItem != XSet.end(); relevanceItem++) {
			auto obseverItem = m_ViewObjMap[*relevanceItem]; 
			actoViewange->EnterView(obseverItem); 
		}
	}
} 
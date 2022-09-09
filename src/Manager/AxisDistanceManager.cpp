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
	ViewRange* actorView = NULL;//首先移动所有的角色
	bool isDelete = false;//当前是否仅仅计算  
	auto iterEnd = m_DelayCalcMoveList.end();
	for (auto item = m_DelayCalcMoveList.begin(); item != iterEnd;item++) { 
		ActorID actorID = item->first; 
		actorView = item->second;
		if (actorView->IsRefreshActorView() == 0) {//移动距离过浅
			continue;
		}
		actorView->RecalcBodyPosition();//重计算角色的位置  
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
	const b2AABB& viewAABB = actoViewRange->GetAABB(type);//获取到对应类型的AABB
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
	const b2AABB& viewAABB = actoViewRange->GetAABB(type);//获取到对应类型的AABB
	b2Vec2 leftMax(viewAABB.lowerBound.x, viewAABB.upperBound.y);//POS_BODY_LIMIT_LEFT_MAX
	b2Vec2 leftMin(viewAABB.lowerBound.x, viewAABB.lowerBound.y);//POS_BODY_LIMIT_LEFT_MIN
	b2Vec2 rightMax(viewAABB.upperBound.x, viewAABB.upperBound.y);//POS_BODY_LIMIT_RIGHT_MAX
	b2Vec2 rightMin(viewAABB.upperBound.x, viewAABB.lowerBound.y); //POS_BODY_LIMIT_RIGHT_MIN  
	b2Vec2* posPoint[PointPosType::POS_BODY_LIMIT_NUM_MAX] = { &leftMax,&leftMin,&rightMax,&rightMin };
	ViewRangeRecordMap& axisCell = m_AxisBodyMap[type];
	ActorID actorID = actoViewRange->GetActor()->ID();
	for (int i = 0; i < PointPosType::POS_BODY_LIMIT_NUM_MAX; i++) {
		b2Vec2& pos = *posPoint[i];
		if (!axisCell.count(*posPoint[i]))//首先判断对应类型是否有当前信息
			continue;
		axisCell[*posPoint[i]]->Erease(actorID, (PointPosType)i);//删除这个单元
		if (axisCell[*posPoint[i]]->IsInValid()) {
			m_ViewCellIdleGenerate.BackObj(axisCell[*posPoint[i]]);
			axisCell.erase(*posPoint[i]);
		}
	}
}

extern ActorID playerID;
//寻找某一个轴的某一范围内的所有单元信息  
void AxisDistanceManager::InquiryAxisPoints(unordered_set<ActorID>& outData,const b2AABB& viewAABB)//偏移单元代表当前要查询的息(必须确保当前被查询点存在于表内)
{ 
	b2Vec2 tempPoint = viewAABB.lowerBound;//获取到当前的视图最低点
	ViewRangeRecordMap::iterator iterEnd = m_AxisBodyMap[PointType::BODY_TYPE].end();//获取到小于等于当前值的东西  
	ViewRangeRecordMap::iterator iter = m_AxisBodyMap[PointType::BODY_TYPE].lower_bound(tempPoint);
	while (iter != iterEnd && iter->first.x <= viewAABB.upperBound.x)//末尾 或者 越界
	{	
		while (iter != iterEnd && (iter->first.y > viewAABB.upperBound.y || iter->first.y < viewAABB.lowerBound.y))//在这个范围内的话,就不停的寻找下一个
		{
			if (iter->first.y > viewAABB.upperBound.y) {
				tempPoint.Set(iter->first.x, 99999);
				iter = m_AxisBodyMap[PointType::BODY_TYPE].upper_bound(tempPoint);//查找x轴
			} 
			if (iter != iterEnd && iter->first.y < viewAABB.lowerBound.y) {
				tempPoint.Set(iter->first.x, viewAABB.lowerBound.y); 
				iter = m_AxisBodyMap[PointType::BODY_TYPE].lower_bound(tempPoint);//查找x轴
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
//重计算
void AxisDistanceManager::CalcViewObj()
{
	//每个角色先移动一下自己,刷新当前的被刷新对象
	for (auto actorItem = m_CalcMoveList.begin(); actorItem != m_CalcMoveList.end(); actorItem++)
		actorItem->second->MoveSelf();//代表自己移动了

	//重新计算自己所见所得
	ViewRange* actoViewange = NULL;
	for (auto item = m_CalcMoveList.begin(); item != m_CalcMoveList.end(); item++) {
		//首先计算当前的操作 
		ActorID actorID = item->first; 
		actoViewange = item->second;
		actoViewange->ClearObserverTable();//清除,然后重新计算
		unordered_set<ActorID> XSet;
		const b2AABB& viewAABB = actoViewange->GetViewAABB();
		InquiryAxisPoints(XSet, viewAABB);//获取到X轴的列表  
		for (auto relevanceItem = XSet.begin(); relevanceItem != XSet.end(); relevanceItem++) {
			auto obseverItem = m_ViewObjMap[*relevanceItem]; 
			actoViewange->EnterView(obseverItem); 
		}
	}
	m_CalcMoveList.clear();
} 
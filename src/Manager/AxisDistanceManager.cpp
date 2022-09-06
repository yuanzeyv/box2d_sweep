#include "Manager/AxisDistanceManager.h" 
using namespace std;    
ViewRange::ViewRange(BodyData* bodyData, AutomaticGenerator<b2AABB>* allocObj, b2Vec2 range) :
	m_ObserverRange(range),
	m_BodyPos(MAX_DISTANCE, MAX_DISTANCE),
	m_Actor(bodyData),
	m_AABBAllocObj(allocObj) {}


void ViewRange::RecalcAABBRange(PointType type)//重新计算视口
{
	b2AABB& aabbGrid = m_BodyAABB[type];
	if (type == PointType::VIEW_TYPE) { 
		aabbGrid.lowerBound.Set(m_BodyPos.x - m_ObserverRange.x, m_BodyPos.y - m_ObserverRange.x);//重新计算视图
		aabbGrid.upperBound.Set(m_BodyPos.x + m_ObserverRange.y, m_BodyPos.y + m_ObserverRange.y);
	} else if(type == PointType::BODY_TYPE) {
		const b2AABB& bodyAABB = m_Actor->GetAABB();//获取到角色的AABB
		memcpy(&m_BodyAABB[type], &bodyAABB, sizeof(b2AABB));//拷贝数据
		RecalcRefreshCondtion();
	}
}  

void ViewRange::CalcSplitAABB(PointType type)//计算分割刚体(AABB会经常算,视图不经常算)
{
	b2AABB& aabbGrid = m_BodyAABB[type];//AABB单元 
	float length = aabbGrid.upperBound.x - aabbGrid.lowerBound.x;//首先获取到当前刚体的AABB的长度
	int count = ceil(length / MAX_OVERFLOW_RANGE);//计算需要分配多少个
	if(!count) count = 1;//最少分配一个 
	int splitCount = count - m_SplitAABBList[type].size();//获取到当前的分割个数
	if (splitCount < 0) {//归还个数
		for (int i = 0; i > splitCount; i--) {
			auto obj = m_SplitAABBList[type].begin();
			m_AABBAllocObj->BackObj(*obj);
			m_SplitAABBList[type].erase(obj);
		}
	} else if (splitCount > 0) {
		for (int i = 0; i > splitCount; i--) 
			m_SplitAABBList[type].push_back(m_AABBAllocObj->RequireObj()); 
	}
	float calcX = m_BodyAABB[type].lowerBound.x;//从左开始,遍历
	for (auto item = m_SplitAABBList[type].begin(); item != m_SplitAABBList[type].end(); item++){
		b2AABB* aabb = *item;
		aabb->lowerBound.Set(calcX, m_BodyAABB[type].lowerBound.y);
		if (calcX + MAX_OVERFLOW_RANGE < m_BodyAABB[type].upperBound.x)
			aabb->upperBound.Set(calcX + MAX_OVERFLOW_RANGE, m_BodyAABB[type].upperBound.y);
		else
			aabb->upperBound.Set(m_BodyAABB[type].upperBound.x, m_BodyAABB[type].upperBound.y);
	}
}  

//自己移动的话
void ViewRange::MoveSelf() {//通知每个观察到自己的人,当前自己移动了
	for (auto item = m_BeObserverMap.begin(); item != m_BeObserverMap.end(); ) {
		if (!item->second->IsContain(this))//如果自己当前还在对方的视野下
			m_BeObserverMap.erase(item++);
		else
			item++;
	}
}

bool ViewRange::IsContain(ViewRange* actor) {//移动的角色询问当前是否还在监听范围内
	ActorID id = actor->m_Actor->ID();
	bool isObserver = this->GetBodyAABB(PointType::VIEW_TYPE).Contains(actor->GetBodyAABB(PointType::BODY_TYPE));//判断两个AABB是否重叠
	if (!isObserver && m_ObserverMap.count(id))//如果当前看得观察到了这个刚体,但现在看不见了
		m_ObserverMap.erase(id);
	return isObserver;//删除自己
}

ViewRange::~ViewRange()
{
	RecycleAABBPoint(PointType::BODY_TYPE);
	RecycleAABBPoint(PointType::VIEW_TYPE);
}


AxisDistanceManager::AxisDistanceManager() {
}
//在使用本函数之前应确保,当前的列表中,再无对应角色
inline void AxisDistanceManager::AdditionDistancePoint(ActorID actorID, PointType addType, const b2AABB& viewRange)
{
	ActorID minActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MIN); //生成最小ID
	ActorID maxActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MAX); //生成最大ID 
	ViewRangeRecordMap& xRangeRecordMap = m_AxisBodyMap[AxisType::X_AXIS];
	ViewRangeRecordMap& yRangeRecordMap = m_AxisBodyMap[AxisType::Y_AXIS]; 
	if (!xRangeRecordMap.count(viewRange.lowerBound.x)) //不存在当前坐标点的话   
		xRangeRecordMap[viewRange.lowerBound.x] = m_ViewCellIdleGenerate.RequireObj();
	if (!xRangeRecordMap.count(viewRange.upperBound.x)) //不存在当前坐标点的话   
		xRangeRecordMap[viewRange.upperBound.x] = m_ViewCellIdleGenerate.RequireObj(); 
	if (!yRangeRecordMap.count(viewRange.lowerBound.y)) //不存在当前坐标点的话   
		yRangeRecordMap[viewRange.lowerBound.y] = m_ViewCellIdleGenerate.RequireObj(); 
	if (!yRangeRecordMap.count(viewRange.upperBound.y)) //不存在当前坐标点的话   
		yRangeRecordMap[viewRange.upperBound.y] = m_ViewCellIdleGenerate.RequireObj();
	(*xRangeRecordMap[viewRange.lowerBound.x])[addType].insert(minActorID);
	(*xRangeRecordMap[viewRange.upperBound.x])[addType].insert(maxActorID);
	(*yRangeRecordMap[viewRange.lowerBound.y])[addType].insert(minActorID);
	(*yRangeRecordMap[viewRange.upperBound.y])[addType].insert(maxActorID);
}
inline void AxisDistanceManager::RemoveDistancePoint(ActorID actorID, PointType type, const b2AABB& viewRange)
{
	ActorID minActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MIN); //生成最小ID
	ActorID maxActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MAX); //生成最大ID 
	ViewRangeRecordMap& xRangeRecordMap = m_AxisBodyMap[AxisType::X_AXIS];//获取到X轴的信息
	ViewRangeRecordMap& yRangeRecordMap = m_AxisBodyMap[AxisType::Y_AXIS];//获取到Y轴的信息
	if (xRangeRecordMap.count(viewRange.lowerBound.x))//删除这个ID对应的X坐标点 
			(*xRangeRecordMap[viewRange.lowerBound.x])[type].erase(minActorID);
	if (xRangeRecordMap.count(viewRange.upperBound.x))//存在当前坐标点的话  
			(*xRangeRecordMap[viewRange.upperBound.x])[type].erase(maxActorID);
	if (yRangeRecordMap.count(viewRange.lowerBound.y))//存在当前坐标点的话  
			(*yRangeRecordMap[viewRange.lowerBound.y])[type].erase(minActorID);
	if (yRangeRecordMap.count(viewRange.upperBound.y))//存在当前坐标点的话  
			(*yRangeRecordMap[viewRange.upperBound.y])[type].erase(maxActorID);
	ABBBRnageRecycle(viewRange.lowerBound.x, xRangeRecordMap);
	ABBBRnageRecycle(viewRange.upperBound.x, xRangeRecordMap);
	ABBBRnageRecycle(viewRange.lowerBound.y, yRangeRecordMap);
	ABBBRnageRecycle(viewRange.upperBound.y, yRangeRecordMap);
}

void AxisDistanceManager::ActorsMove()
{ 
	ViewRange* actorView = NULL;//首先移动所有的角色
	bool OnlyCalculate = false;//当前是否仅仅计算
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end();) {
		ActorID actorID = *item;
		OnlyCalculate = false;
		if (!m_ViewObjMap.count(actorID))//对象不存在,直接删除
			goto erase;
		actorView = m_ViewObjMap[actorID];
		if (actorView->IsRefreshActorView() == 0 )//移动距离过浅
			goto calc_erase;//重新计算位置,但是却
		actorView->RecalcBodyPosition();//重计算角色的位置
		OnlyCalculate = true;
	calc_erase:
		RemoveDistanceAABB(actorView, PointType::BODY_TYPE);//删除body
		RemoveDistanceAABB(actorView, PointType::VIEW_TYPE);//删除view 
		actorView->RecalcBodyAABB(PointType::BODY_TYPE);//重新计算当前的碰撞盒子
		actorView->RecalcBodyAABB(PointType::VIEW_TYPE);//重新计算当前的视图盒子 
		AdditionDistanceAABB(actorView, PointType::BODY_TYPE);//将现有的添加上去
		AdditionDistanceAABB(actorView, PointType::VIEW_TYPE);//将现有的添加上去 
		item++;
		if (OnlyCalculate) continue;
	erase:
		m_DelayCalcMoveList.erase(item++);
	}
}
//寻找某一个轴的某一范围内的所有单元信息
bool AxisDistanceManager::InquiryAxisPoints(unordered_set<ActorID>& outData, float findKey, PointType type, AxisType axis, float inquiryDistance, float leftOffset, float rightOffset)//偏移单元代表当前要查询的息(必须确保当前被查询点存在于表内)
{
	if (!m_AxisBodyMap[axis].count(findKey)) return false;
	auto axisMap = m_AxisBodyMap[axis];
	auto beginIterator = axisMap.find(findKey);//正向
	std::map<float, ViewRangeTypeSet*>::reverse_iterator rBeginITerator(beginIterator);//反向
	float rightDistance = inquiryDistance + rightOffset;//查询的最终距离
	for (auto item = beginIterator; item != axisMap.end() && item->first <= rightDistance; item++) {
		auto typeMap = (*item->second)[type];
		for (auto distanceItem = typeMap.begin(); distanceItem != typeMap.end(); distanceItem++)
			outData.insert(GET_ACTOR_ID(*distanceItem));
	}
	//首先获取到当前寻找到的位置
	float leftDistance = inquiryDistance - leftOffset;
	if (leftOffset >= inquiryDistance) goto out; 
	rBeginITerator++;//之前的计算中,已经计算了自己
	for (auto item = rBeginITerator; item != axisMap.rend() && item->first >= leftDistance; item++) {
		auto typeMap = (*item->second)[type];
		for (auto distanceItem = typeMap.begin(); distanceItem != typeMap.end(); distanceItem++)
			outData.insert(GET_ACTOR_ID(*distanceItem));
	}
out:
	return true;
}
//重计算
inline void AxisDistanceManager::CalcViewObj()
{
	//每个角色先移动一下自己,刷新当前的被刷新对象
	for (auto actorIDItem = m_DelayCalcMoveList.begin(); actorIDItem != m_DelayCalcMoveList.end();actorIDItem++) {  
		ViewRange* actoViewange = m_ViewObjMap[*actorIDItem];
		actoViewange->MoveSelf();//代表自己移动了
	}

	//重新计算自己所见所得
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end();) {
		//首先计算当前的操作 
		ActorID actorID = *item;
		ViewRange* actoViewange = m_ViewObjMap[actorID];
		actoViewange->ClearObserverTable();//清除,然后重新计算
		unordered_set<ActorID> XSet;
		const b2AABB& viewAABB = actoViewange->GetBodyAABB(PointType::VIEW_TYPE);
		InquiryAxisPoints(XSet, viewAABB.lowerBound.x, PointType::BODY_TYPE, AxisType::X_AXIS, viewAABB.upperBound.x - viewAABB.lowerBound.x, 0,MAX_OVERFLOW_RANGE);//获取到X轴的列表
		InquiryAxisPoints(XSet, viewAABB.lowerBound.y, PointType::BODY_TYPE, AxisType::Y_AXIS, viewAABB.upperBound.y - viewAABB.lowerBound.y, 0, MAX_OVERFLOW_RANGE);//获取到X轴的列表 
		//所有关联的角色统统计算一下距离

		for (auto relevanceItem = XSet.begin(); relevanceItem != XSet.end(); relevanceItem++) {
			auto obseverItem = m_ViewObjMap[*relevanceItem];
			//计算AABB
			if (obseverItem->GetBodyAABB(PointType::BODY_TYPE).Contains(actoViewange->GetBodyAABB(PointType::VIEW_TYPE)))
			{
				actoViewange->EnterView(obseverItem);
			}
		}
	}
}
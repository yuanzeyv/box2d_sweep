#include "Manager/AxisDistanceManager.h" 

#include<queue>
using namespace std;    
ViewRange::ViewRange(BodyData* bodyData, b2Vec2 range) :
	m_ObserverRange(range),
	m_BodyPos(MAX_DISTANCE, MAX_DISTANCE),
	m_Actor(bodyData), 
	m_OffsetCondtion(-9999,-9999),
	m_ViewObserverStateGenerate(20)
{
	m_FrontObserverPoint = &m_ObserverArr[0];//第一个原始代表上一帧可以看到的
	m_ObserverPoint = &m_ObserverArr[1];//第一个原始代表上一帧可以看到的 
	m_FrontBeObserverPoint = &m_BeObserverArr[0];//第一个原始代表上一帧可以看到的
	m_BeObserverPoint = &m_BeObserverArr[1];//第一个原始代表上一帧可以看到的 
}
void ViewRange::ChangeObserverStatus(ViewRange* ObserverActor, bool status)
{
	ActorID actorID = ObserverActor->GetActor()->ID();
	auto itor = lower_bound(m_BeObserverPoint->begin(), m_BeObserverPoint->end(), actorID, [](ViewObserverState* actor, ActorID id)->bool {return actor->m_ViewRange->GetActor()->ID() < id; });
	ViewObserverState* statePoint = NULL;
	if (itor != m_BeObserverPoint->end() && (*itor)->m_ViewRange->GetActor()->ID() == actorID) {
		statePoint = *itor;
	}
	else if (m_ExtraObserverArr.count(actorID)) {
		statePoint = m_ExtraObserverArr[actorID];
	}
	if (statePoint == NULL && status == 1)
		return;
	if (statePoint == NULL) {
		statePoint = m_ViewObserverStateGenerate.RequireObj();
		statePoint->m_IsVisible = 0;
		statePoint->m_ViewRange = ObserverActor;
		m_ExtraObserverArr[actorID] = statePoint;
		return;
	}
	statePoint->m_IsVisible = status;
}

void ViewRange::ChangeBeObserverStatus(ViewRange* beObserverActor, bool status)
{
	ActorID actorID = beObserverActor->GetActor()->ID();
	auto itor = lower_bound(m_BeObserverPoint->begin(), m_BeObserverPoint->end(), actorID, [](ViewObserverState* actor, ActorID id)->bool {return actor->m_ViewRange->GetActor()->ID() < id; });
	ViewObserverState * statePoint = NULL;
	if (itor != m_BeObserverPoint->end() && (*itor)->m_ViewRange->GetActor()->ID() == actorID) {
		statePoint = *itor;
	}else if (m_ExtraBeObserverArr.count(actorID)) {
		statePoint = m_ExtraBeObserverArr[actorID];
	}
	if (statePoint == NULL && status == 1)
		return;
	if (statePoint == NULL) {
		statePoint = m_ViewObserverStateGenerate.RequireObj();
		statePoint->m_IsVisible = 0;
		statePoint->m_ViewRange = beObserverActor;
		m_ExtraBeObserverArr[actorID] = statePoint;
		return;
	}
	statePoint->m_IsVisible = status;
}
void ViewRange::ClearExtraObserverArr()//删除所有额外的观察对象
{
	for (auto item = m_ExtraObserverArr.begin();item != m_ExtraObserverArr.end();item++) {
		m_ViewObserverStateGenerate.BackObj(item->second);
	}
	m_ExtraObserverArr.clear();
}

void ViewRange::ClearExtraBeObserverArr()//删除所有额外的观察对象
{
	for (auto item = m_ExtraBeObserverArr.begin();item != m_ExtraBeObserverArr.end();item++) {
		m_ViewObserverStateGenerate.BackObj(item->second);
	}
	m_ExtraBeObserverArr.clear();
}

AxisMap::~AxisMap() {
	for (int i = 0; i < PointType::MAX_POINT_TYPE;i++) {
		for (int j = 0;j < m_RecordArray[i].size();j++) {
			m_InPosActorsGenerate.BackObj(m_RecordArray[i][j]);
		}
		m_XAxisActors[i].clear();
		m_RecordArray[i].resize(0);
	}
}

void AxisMap::AddtionViewRange(PointType type, ViewRange* actor, b2AABB& aabb)
{
	float lowX = aabb.lowerBound.x;//保存低点的X 
	ActorID actorId = actor->GetActor()->ID();
	if (m_XAxisActors[type].count(lowX) == 0)
		m_XAxisActors[type][lowX] = skiplist_new();
	auto jumpList = m_XAxisActors[type][lowX];
	if (!skiplist_search(jumpList, actorId)){//插入
		skiplist_insert(jumpList, actorId,(int64_t)actor);
	}  
}
 
void AxisMap::DeleteViewRange(PointType type,ViewRange* actor, b2AABB& aabb)
{
	float lowX = aabb.lowerBound.x;//保存低点的X   
	ActorID actorId = actor->GetActor()->ID(); 
	if (m_XAxisActors[type].count(lowX) == 0)
		return;
	auto jumpList = m_XAxisActors[type][lowX];
	skiplist_remove(jumpList, actorId);
	if (jumpList->count == 0) {
		skiplist_delete(jumpList);
		m_XAxisActors[type].erase(lowX);
	}  
}
//准备计算了,初始化当前的数据信息
void AxisMap::ReadyInitActorData()
{ 
	for (int i = 0; i < PointType::MAX_POINT_TYPE;i++) {
		int index = 0;
		size_t size = m_XAxisActors[i].size();//获取到当前的大小
		size_t nowSize = m_RecordArray[i].size();//获取到当前的大小
		if (size != nowSize) {
			if ((size - nowSize) > 0) {//需要添加的情况
				m_RecordArray[i].resize(size);
				InPosActors** actorData = m_RecordArray[i].data();
				for (int index = nowSize;index < size;index++)
					actorData[index] = m_InPosActorsGenerate.RequireObj();
			}
			else {
				InPosActors** actorData = m_RecordArray[i].data();
				for (int index = nowSize;index < size;index++)
					m_InPosActorsGenerate.BackObj(actorData[index]);
				m_RecordArray[i].resize(size);
			}
		}
		InPosActors** actorData = m_RecordArray[i].data();//重新设置数组的大小
		auto itorEnd = m_XAxisActors[i].end();
		for (auto actorSet = m_XAxisActors[i].begin(); actorSet != itorEnd; actorSet++, index++) {
			actorData[index]->m_PositionX = actorSet->first;
			actorData[index]->m_SkipList = actorSet->second;
		}
	}
} 
static bool ViewRangeIDCompare(ViewRange* node1, ViewRange* node2)
{ 
	return node1->GetActor()->ID() < node2->GetActor()->ID();
} 
// 获取到范围内的所有合适的角色
int AxisMap::SetRangeActors(PointType type, ViewRange* actor,std::vector<ViewObserverState*>& outData, const b2AABB& range)
{ 
	InPosActors posActor; 
	posActor.m_PositionX = range.upperBound.x; 
	auto lowPointItorSize = std::lower_bound(m_RecordArray[type].begin(), m_RecordArray[type].end(), &posActor, InPosActors()) - m_RecordArray[type].begin(); //先判断了X,在判断一遍Y
	std::vector<ViewRange*> tempData; 
	InPosActors** recordData = m_RecordArray[type].data();
	ViewRange* viewRangeTemp = NULL;
	for (int i = 0; i < lowPointItorSize; i++) {
		skiplist* skipList = recordData[i]->m_SkipList; 
		struct sk_link* pos = &skipList->head[0];
		struct sk_link* end = &skipList->head[0];
		pos = pos->next;
		for (; pos != end; pos = pos->next) { 
			viewRangeTemp = (ViewRange*)list_entry(pos, struct skipnode, link[0])->value;
			b2AABB& AABB = viewRangeTemp->GetBodyAABB();
			if (AABB.lowerBound.x > range.upperBound.x || AABB.lowerBound.y > range.upperBound.y || range.lowerBound.x > AABB.upperBound.x || range.lowerBound.y > AABB.upperBound.y)
				continue;
			tempData.push_back(viewRangeTemp); 
		} 
	}
	ViewRange** tempDataArr = tempData.data();
	sort(tempData.begin(), tempData.end(), ViewRangeIDCompare); //排序之后,便可用了 
	size_t tempSize = tempData.size();//获取到当前的大小
	size_t nowSize = outData.size();//获取到当前的大小
	if (nowSize < tempSize)
		outData.resize(tempSize);//对其扩容
	ViewObserverState** outDataArr = outData.data();
	size_t loopCount = max(tempSize, nowSize);
	for (int i = 0; i < loopCount;i++) {
		if (i < tempSize) {//有用的话
			if (!outDataArr[i])//退回
				outDataArr[i] = actor->m_ViewObserverStateGenerate.RequireObj();
			outDataArr[i]->m_IsVisible = 0;//可见
			outDataArr[i]->m_ViewRange = tempDataArr[i];
		} else {
			if(outDataArr[i])//退回
				actor->m_ViewObserverStateGenerate.BackObj(outDataArr[i]);
		} 
	}
	if (nowSize > tempSize)
		outData.resize(tempSize);//对其扩容 
	return outData.size();
} 

void AxisDistanceManager::ActorsMove()
{
	ViewRange* actorView = NULL;//首先移动所有的角色
	bool isDelete = false;//当前是否仅仅计算  
	auto iterEnd = m_DelayCalcMoveList.end();//防止重复获取到end
	for (auto item = m_DelayCalcMoveList.begin(); item != iterEnd;item++) { //循环遍历到所有的移动的角色
		ActorID actorID = item->first; //获取到角色ID
		actorView = item->second;//获取到角色的视图数据
		if(actorView->IsRefreshActorView() == 0)//移动距离过浅
			continue;
		actorView->RecalcBodyPosition();//重计算角色的位置   
		m_AxisBodyMap->DeleteViewRange(PointType::BODY_TYPE, actorView, actorView->GetBodyAABB());
		m_AxisBodyMap->DeleteViewRange(PointType::VIEW_TYPE, actorView, actorView->GetViewAABB());
		actorView->RecalcBodyAABB();
		actorView->RecalcViewAABB();
		m_AxisBodyMap->AddtionViewRange(PointType::BODY_TYPE, actorView, actorView->GetBodyAABB());
		m_AxisBodyMap->AddtionViewRange(PointType::VIEW_TYPE, actorView, actorView->GetViewAABB());
		m_CalcMoveList[actorID] =item->second;
	}  
	m_DelayCalcMoveList.clear(); 
} 
inline static bool ViewRangePointComapre(ViewRange* v1, ViewRange* v2)
{ 
	return v1->GetActor()->ID() < v2->GetActor()->ID();
}  
void AxisDistanceManager::CalcObserver(vector<ViewRange*>& moveRangeArr)//重计算 
{
	//取得对象上一帧所观察的到的所有的信息
	vector<ViewRange*> frontObserverCalcArr;
	//取得对象这一帧所观察的到的所有的信息
	vector<ViewRange*> observerCalcArr; 
	vector<ViewRange*> differenceInVisibleCalcArr;//通过与可见列表取差集,剔除无变化的人员信息
	vector<ViewRange*> differenceVisibleCalcArr;//通过与可见列表取差集,剔除无变化的人员信息 
	vector<ViewRange*> finalDifferenceArr;//通过与可见列表取差集,剔除无变化的人员信息 
	int moveSize = moveRangeArr.size(); 
	ViewRange** moveArrData = moveRangeArr.data();
	ViewRange* actoViewange = NULL;
	ViewRange** observerArr = NULL;
	int index;
	for (int i = 0;i < moveSize;i++) { 
		actoViewange = moveArrData[i];
		frontObserverCalcArr.resize(actoViewange->m_FrontObserverPoint->size() + actoViewange->m_ExtraObserverArr.size());
		//取得对象上一帧所观察的到的所有的信息
		observerArr = frontObserverCalcArr.data();
		index = 0;
		for (int pos = 0;pos < actoViewange->m_FrontObserverPoint->size();pos++) {
			if (actoViewange->m_FrontObserverPoint->at(pos)->m_IsVisible == 1)
				continue;
			observerArr[index++] = actoViewange->m_FrontObserverPoint->at(pos)->m_ViewRange;//可见才加入,否则视为被删除的
		}
		for (auto item = actoViewange->m_ExtraObserverArr.begin();item != actoViewange->m_ExtraObserverArr.end();item++) {
			if (item->second->m_IsVisible == 1)
				continue;
			observerArr[index++] = item->second->m_ViewRange;//可见才加入,否则视为被删除的
		}
		frontObserverCalcArr.resize(index);
		//取得对象这一帧所观察的到的所有的信息 
		observerCalcArr.resize(actoViewange->m_ObserverPoint->size());
		observerArr = observerCalcArr.data();
		for (int pos = 0;pos < actoViewange->m_ObserverPoint->size();pos++) {
			observerArr[pos] = actoViewange->m_ObserverPoint->at(pos)->m_ViewRange;
		}
		//前一帧 与 本帧取差集. 得到应该被设置为隐藏的单元
		set_difference(frontObserverCalcArr.begin(), frontObserverCalcArr.end(), observerCalcArr.begin(), observerCalcArr.end(), inserter(differenceInVisibleCalcArr, differenceInVisibleCalcArr.begin()), ViewRangePointComapre);
		set_difference(differenceInVisibleCalcArr.begin(), differenceInVisibleCalcArr.end(), moveRangeArr.begin(), moveRangeArr.end(), inserter(finalDifferenceArr, finalDifferenceArr.begin()), ViewRangePointComapre);
		for (auto item = finalDifferenceArr.begin();item != finalDifferenceArr.end();item++) {
			actoViewange->ChangeBeObserverStatus(*item, 1); //设置当前对象未可视状态
		}
		set_difference(observerCalcArr.begin(), observerCalcArr.end(), frontObserverCalcArr.begin(), frontObserverCalcArr.end(), inserter(differenceVisibleCalcArr, differenceVisibleCalcArr.begin()), ViewRangePointComapre);
		set_difference(differenceVisibleCalcArr.begin(), differenceVisibleCalcArr.end(), moveRangeArr.begin(), moveRangeArr.end(), inserter(finalDifferenceArr, finalDifferenceArr.begin()), ViewRangePointComapre);
		for (auto item = finalDifferenceArr.begin();item != finalDifferenceArr.end();item++) {
			actoViewange->ChangeBeObserverStatus(*item, 0); //设置当前对象未可视状态
		}
		observerCalcArr.clear();
		differenceInVisibleCalcArr.clear();
		differenceVisibleCalcArr.clear();
		finalDifferenceArr.clear();
		actoViewange->ClearExtraBeObserverArr();
	}
}

void AxisDistanceManager::CalcBeObserver(vector<ViewRange*>& moveRangeArr)//重计算 
{
	//取得对象上一帧所观察的到的所有的信息
	vector<ViewRange*> frontBeObserverCalcArr;
	//取得对象这一帧所观察的到的所有的信息
	vector<ViewRange*> BeObserverCalcArr;
	vector<ViewRange*> differenceInVisibleCalcArr;//通过与可见列表取差集,剔除无变化的人员信息
	vector<ViewRange*> differenceVisibleCalcArr;//通过与可见列表取差集,剔除无变化的人员信息 
	vector<ViewRange*> finalDifferenceArr;//通过与可见列表取差集,剔除无变化的人员信息 
	int moveSize = moveRangeArr.size();
	ViewRange** moveArrData = moveRangeArr.data();
	ViewRange* actoViewange = NULL;
	ViewRange** observerArr = NULL;
	int index;
	for (int i = 0;i < moveSize;i++) {
		actoViewange = moveArrData[i];
		frontBeObserverCalcArr.resize(actoViewange->m_FrontObserverPoint->size() + actoViewange->m_ExtraObserverArr.size());
		//取得对象上一帧所观察的到的所有的信息
		observerArr = frontBeObserverCalcArr.data();
		index = 0;
		for (int pos = 0;pos < actoViewange->m_FrontObserverPoint->size();pos++) {
			if (actoViewange->m_FrontObserverPoint->at(pos)->m_IsVisible == 1)
				continue;
			observerArr[index++] = actoViewange->m_FrontObserverPoint->at(pos)->m_ViewRange;//可见才加入,否则视为被删除的
		}
		for (auto item = actoViewange->m_ExtraObserverArr.begin();item != actoViewange->m_ExtraObserverArr.end();item++) {
			if (item->second->m_IsVisible == 1)
				continue;
			observerArr[index++] = item->second->m_ViewRange;//可见才加入,否则视为被删除的
		}
		frontBeObserverCalcArr.resize(index);
		//取得对象这一帧所观察的到的所有的信息 
		BeObserverCalcArr.resize(actoViewange->m_ObserverPoint->size());
		observerArr = BeObserverCalcArr.data();
		for (int pos = 0;pos < actoViewange->m_ObserverPoint->size();pos++) {
			observerArr[pos] = actoViewange->m_ObserverPoint->at(pos)->m_ViewRange;
		}
		//前一帧 与 本帧取差集. 得到应该被设置为隐藏的单元
		set_difference(frontBeObserverCalcArr.begin(), frontBeObserverCalcArr.end(), BeObserverCalcArr.begin(), BeObserverCalcArr.end(), inserter(differenceInVisibleCalcArr, differenceInVisibleCalcArr.begin()), ViewRangePointComapre);
		set_difference(differenceInVisibleCalcArr.begin(), differenceInVisibleCalcArr.end(), moveRangeArr.begin(), moveRangeArr.end(), inserter(finalDifferenceArr, finalDifferenceArr.begin()), ViewRangePointComapre);
		for (auto item = finalDifferenceArr.begin();item != finalDifferenceArr.end();item++) {
			actoViewange->ChangeObserverStatus(*item, 1); //设置当前对象未可视状态
		} 
		set_difference(BeObserverCalcArr.begin(), BeObserverCalcArr.end(), frontBeObserverCalcArr.begin(), frontBeObserverCalcArr.end(), inserter(differenceVisibleCalcArr, differenceVisibleCalcArr.begin()), ViewRangePointComapre);
		set_difference(differenceVisibleCalcArr.begin(), differenceVisibleCalcArr.end(), moveRangeArr.begin(), moveRangeArr.end(), inserter(finalDifferenceArr, finalDifferenceArr.begin()), ViewRangePointComapre);
		for (auto item = finalDifferenceArr.begin();item != finalDifferenceArr.end();item++) {
			actoViewange->ChangeObserverStatus(*item, 0); //设置当前对象未可视状态
		}
		BeObserverCalcArr.clear();
		differenceInVisibleCalcArr.clear();
		differenceVisibleCalcArr.clear();
		finalDifferenceArr.clear();
		actoViewange->ClearExtraObserverArr();
	}
}
//重计算
void AxisDistanceManager::CalcViewObj()
{
	//准备计算数据
	m_AxisBodyMap->ReadyInitActorData();
	//获取到当前的移动列表的数组
	int index = 0;
	int moveSize = m_CalcMoveList.size();
	vector<ViewRange*> moveArr(moveSize);
	ViewRange** moveArrData = moveArr.data();
	for (auto item = m_CalcMoveList.begin();item != m_CalcMoveList.end();item++)
		moveArrData[index++] = item->second;  
	//重新设置所有的玩家的视野英雄
	ViewRange* actoViewange = NULL;
	for (int i = 0;i < moveSize;i++) { 
		moveArrData[i]->ShiftObserverPoint();//保存上一帧数据
		m_AxisBodyMap->SetRangeActors(PointType::BODY_TYPE, moveArrData[i], *moveArrData[i]->m_ObserverPoint, moveArrData[i]->GetViewAABB());//重新计算本帧观察
		moveArrData[i]->ShiftBeObserverPoint();//保存上一帧数据
		m_AxisBodyMap->SetRangeActors(PointType::VIEW_TYPE, moveArrData[i], *moveArrData[i]->m_BeObserverPoint, moveArrData[i]->GetBodyAABB());//取得当前有谁观察了我
	}
	CalcBeObserver(moveArr);
	CalcObserver(moveArr);
	m_CalcMoveList.clear();   
}
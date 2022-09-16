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
	auto observerDataPoint = m_ObserverPoint->data();
	auto findData = lower_bound(observerDataPoint, observerDataPoint + m_ObserverPoint->size(), actorID, [](ViewObserverState* actor, ActorID id)->bool {return actor->m_ViewRange->GetActor()->ID() < id; });
	ViewObserverState* statePoint = NULL;
	if ((findData - observerDataPoint) != m_ObserverPoint->size() && (*findData)->m_ViewRange->GetActor()->ID() == actorID) {
		statePoint = *findData;
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
	auto observerDataPoint = m_BeObserverPoint->data();
	auto findData = lower_bound(observerDataPoint, observerDataPoint + m_BeObserverPoint->size(), actorID, [](ViewObserverState* actor, ActorID id)->bool {return actor->m_ViewRange->GetActor()->ID() < id; });
	ViewObserverState * statePoint = NULL;
	if ((findData - observerDataPoint) != m_BeObserverPoint->size() && (*findData)->m_ViewRange->GetActor()->ID() == actorID) {
		statePoint = *findData;
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
	float upX = aabb.upperBound.x;//保存低点的X 
	ActorID actorId = actor->GetActor()->ID();
	if (m_XAxisActors[type].count(upX) == 0)
		m_XAxisActors[type][upX] = skiplist_new();
	auto jumpList = m_XAxisActors[type][upX];
	if (!skiplist_search(jumpList, actorId)){//插入
		skiplist_insert(jumpList, actorId,(int64_t)actor);
	}  
}
 
void AxisMap::DeleteViewRange(PointType type,ViewRange* actor, b2AABB& aabb)
{
	float upX = aabb.upperBound.x;//保存低点的X   
	ActorID actorId = actor->GetActor()->ID(); 
	if (m_XAxisActors[type].count(upX) == 0)
		return;
	auto jumpList = m_XAxisActors[type][upX];
	skiplist_remove(jumpList, actorId);
	if (jumpList->count == 0) {
		skiplist_delete(jumpList);
		m_XAxisActors[type].erase(upX);
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
int AxisMap::SetRangeActors(PointType type, PointType compareType, ViewRange* actor,std::vector<ViewObserverState*>& outData, const b2AABB& range)
{ 
	InPosActors posActor; 
	posActor.m_PositionX = range.lowerBound.x; 
	auto lowPointItorSize = std::lower_bound(m_RecordArray[type].begin(), m_RecordArray[type].end(), &posActor, InPosActors()) - m_RecordArray[type].begin(); //先判断了X,在判断一遍Y
	InPosActors** recordData = m_RecordArray[type].data();
	size_t RecordArraySize =  m_RecordArray[type].size();
	size_t realSize = 0; 
	for (int i = lowPointItorSize; i < RecordArraySize; i++) {
		skiplist* skipList = recordData[i]->m_SkipList;
		realSize += skipList->count;
	}
	std::vector<ViewRange*> tempData(realSize);
	ViewRange** tempDataPoint = tempData.data();
	ViewRange* viewRangeTemp = NULL;
	realSize = 0;
	for (int i = lowPointItorSize; i < RecordArraySize; i++) {
		skiplist* skipList = recordData[i]->m_SkipList; 
		struct sk_link* pos = &skipList->head[0];
		struct sk_link* end = &skipList->head[0];
		pos = pos->next;
		for (; pos != end; pos = pos->next) { 
			viewRangeTemp = (ViewRange*)list_entry(pos, struct skipnode, link[0])->value; 
			b2AABB& AABB = PointType::BODY_TYPE == compareType ? viewRangeTemp->GetBodyAABB ():viewRangeTemp->GetViewAABB();
			if (AABB.lowerBound.y > range.upperBound.y || AABB.lowerBound.x > range.upperBound.x || range.lowerBound.y > AABB.upperBound.y)
				continue;
			tempDataPoint[realSize++] = viewRangeTemp;
		} 
	} 
	 
	if(realSize != tempData.size())
		tempData.resize(realSize); 
	sort(tempDataPoint, tempDataPoint + realSize, ViewRangeIDCompare);//视图内观察的对象也不会有太多
	size_t nowSize = outData.size();//获取到当前的大小
	if (nowSize < realSize)
		outData.resize(realSize);
	ViewObserverState** outDataArr = outData.data();
	size_t loopCount = max(realSize, nowSize);
	for (int i = 0; i < loopCount;i++) {
		if (i < realSize) {//有用的话
			if (!outDataArr[i]) outDataArr[i] = actor->m_ViewObserverStateGenerate.RequireObj();
			outDataArr[i]->m_IsVisible = 0;//可见
			outDataArr[i]->m_ViewRange = tempDataPoint[i];
		} else if(outDataArr[i])
			actor->m_ViewObserverStateGenerate.BackObj(outDataArr[i]);
	}
	if (nowSize > realSize)
		outData.resize(realSize);//对其扩容 
	return outData.size();
} 

void AxisDistanceManager::ActorsMove()
{
	ViewRange* actorView = NULL;//首先移动所有的角色
	bool isDelete = false;//当前是否仅仅计算  
	auto iterEnd = m_DelayCalcMoveList.end();//防止重复获取到end
	size_t moveCount = 0;
	size_t delayCount = m_DelayCalcMoveList.size();
	m_CalcMoveList.resize(delayCount);
	ViewRange** moveListData = m_CalcMoveList.data();
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
		moveListData[moveCount++] = item->second;
	}  
	if(delayCount != moveCount)
		m_CalcMoveList.resize(moveCount);//重新设置大小
	m_DelayCalcMoveList.clear(); 
} 
inline static bool ViewRangePointComapre(ViewRange* v1, ViewRange* v2)
{ 
	return v1->GetActor()->ID() < v2->GetActor()->ID();
} 

inline static void DifferenceVector(vector<vector<ViewRange*>*> differnceList, vector<ViewRange*>& outData) {
	//第一个与 第二个 第三个 连续求差集
	vector<ViewRange*>** typeArr = differnceList.data();
	size_t size = differnceList.size();
	vector<ViewRange*>* orign = typeArr[0];
	if (orign->size() == 0) {
		outData.resize(0);
		return;
	}
	vector<ViewRange*> tempArr[2];
	int pointArr[2] = { 0,1 };//0是当前  1是前一个
	for (int i = 1;i < size;i++) {
		int nowPoint = pointArr[0];
		tempArr[nowPoint].resize(max(orign->size(), differnceList[i]->size()));
		tempArr[nowPoint].resize(set_difference(orign->data(), orign->data() + orign->size(),
			differnceList[i]->data(), differnceList[i]->data() + differnceList[i]->size(), tempArr[pointArr[0]].data(), ViewRangePointComapre) - tempArr[nowPoint].data());
		orign = &tempArr[nowPoint]; 
		pointArr[0] = pointArr[1];
		pointArr[1] = nowPoint;
	}
	outData.resize(orign->size());
	if(outData.size() != 0)
		copy(orign->data(), orign->data() + orign->size(), outData.data());
}
void AxisDistanceManager::CalcObserver(vector<ViewRange*>& moveRangeArr)//重计算 
{
	//取得对象上一帧所观察的到的所有的信息
	vector<ViewRange*> frontObserverCalcArr;
	//取得对象这一帧所观察的到的所有的信息
	vector<ViewRange*> observerCalcArr;  
	int moveSize = moveRangeArr.size(); 
	ViewRange** moveArrData = moveRangeArr.data();
	ViewRange* actoViewange = NULL;
	vector<ViewRange*> finalDifferenceArr;//通过与可见列表取差集,剔除无变化的人员信息 
	for (int i = 0;i < moveSize;i++) { 
		actoViewange = moveArrData[i];
		size_t frontSize = actoViewange->m_FrontObserverPoint->size();
		frontObserverCalcArr.resize(frontSize + actoViewange->m_ExtraObserverArr.size());
		ViewRange** tempFrontObserverArr = frontObserverCalcArr.data(); 
		ViewObserverState** frontObserverArr = actoViewange->m_FrontObserverPoint->data();
		size_t observerSize = 0;
		for (int pos = 0;pos < frontSize;pos++) {
			if (frontObserverArr[pos]->m_IsVisible == 1)
				continue;
			tempFrontObserverArr[observerSize++] = frontObserverArr[pos]->m_ViewRange;//可见才加入,否则视为被删除的
		}
		size_t midIndex = observerSize ;
		for (auto item = actoViewange->m_ExtraObserverArr.begin();item != actoViewange->m_ExtraObserverArr.end();item++) {
			if (item->second->m_IsVisible == 1)
				continue;
			tempFrontObserverArr[observerSize++] = item->second->m_ViewRange;//可见才加入,否则视为被删除的
		}
		frontObserverCalcArr.resize(observerSize);
		tempFrontObserverArr = frontObserverCalcArr.data();
		inplace_merge(tempFrontObserverArr, tempFrontObserverArr + midIndex, tempFrontObserverArr + observerSize, ViewRangePointComapre);
		//取得对象这一帧所观察的到的所有的信息 
		observerSize = actoViewange->m_ObserverPoint->size();
		observerCalcArr.resize(observerSize);
		ViewObserverState** actorObserverArr = actoViewange->m_ObserverPoint->data();
		ViewRange**  nowObserverArr = observerCalcArr.data();
		for (int pos = 0;pos < observerSize;pos++) {
			nowObserverArr[pos] = actorObserverArr[pos]->m_ViewRange;
		}
		//前一帧 与 本帧取差集. 得到应该被设置为隐藏的单元 
		DifferenceVector({ &frontObserverCalcArr ,&observerCalcArr ,&moveRangeArr }, finalDifferenceArr); 
		auto finalDiffernceArrData = finalDifferenceArr.data();
		auto finalSize = finalDifferenceArr.size();
		for(int i = 0;i < finalSize;i++) 
			finalDiffernceArrData[i]->ChangeBeObserverStatus(actoViewange, 1); //这个对象将不在被 acto所观察

		DifferenceVector({ &observerCalcArr ,&frontObserverCalcArr ,&moveRangeArr }, finalDifferenceArr); 
		finalDiffernceArrData = finalDifferenceArr.data();
		finalSize = finalDifferenceArr.size();
		for (int i = 0;i < finalSize;i++)
			finalDiffernceArrData[i]->ChangeBeObserverStatus(actoViewange,0); //这个对象将不在被 acto所观察
		actoViewange->ClearExtraBeObserverArr();
	}
}

void AxisDistanceManager::CalcBeObserver(vector<ViewRange*>& moveRangeArr)//重计算 
{
	//取得对象上一帧所观察的到的所有的信息
	vector<ViewRange*> frontBeObserverCalcArr;
	//取得对象这一帧所观察的到的所有的信息
	vector<ViewRange*> beObserverCalcArr; 
	int moveSize = moveRangeArr.size();
	ViewRange** moveArrData = moveRangeArr.data();
	ViewRange* actoViewange = NULL;
	vector<ViewRange*> finalDifferenceArr;//通过与可见列表取差集,剔除无变化的人员信息 
	for (int i = 0;i < moveSize;i++) {
		actoViewange = moveArrData[i];
		size_t frontSize = actoViewange->m_FrontBeObserverPoint->size();
		frontBeObserverCalcArr.resize(actoViewange->m_FrontBeObserverPoint->size() + actoViewange->m_ExtraBeObserverArr.size());
		ViewRange** tempFrontObserverArr = frontBeObserverCalcArr.data();
		ViewObserverState** frontObserverArr = actoViewange->m_FrontBeObserverPoint->data();
		size_t beObserverSize = 0;

		for (int pos = 0;pos < frontSize;pos++) {
			if (frontObserverArr[pos]->m_IsVisible == 1)
				continue;
			tempFrontObserverArr[beObserverSize++] = frontObserverArr[pos]->m_ViewRange;//可见才加入,否则视为被删除的
		}
		size_t midIndex = beObserverSize;
		for (auto item = actoViewange->m_ExtraBeObserverArr.begin();item != actoViewange->m_ExtraBeObserverArr.end();item++) {
			if (item->second->m_IsVisible == 1)
				continue;
			tempFrontObserverArr[beObserverSize++] = item->second->m_ViewRange;//可见才加入,否则视为被删除的
		}
		frontBeObserverCalcArr.resize(beObserverSize);
		tempFrontObserverArr = frontBeObserverCalcArr.data();
		inplace_merge(tempFrontObserverArr, tempFrontObserverArr + midIndex, tempFrontObserverArr + beObserverSize, ViewRangePointComapre);
		//取得对象这一帧所观察的到的所有的信息 
		beObserverSize = actoViewange->m_BeObserverPoint->size();
		beObserverCalcArr.resize(beObserverSize);
		ViewObserverState** actorBeObserverArr = actoViewange->m_BeObserverPoint->data();
		ViewRange** nowBeObserverArr = beObserverCalcArr.data();
		for (int pos = 0;pos < beObserverSize;pos++) {
			nowBeObserverArr[pos] = actorBeObserverArr[pos]->m_ViewRange;
		}
		//前一帧 与 本帧取差集. 得到应该被设置为隐藏的单元
		DifferenceVector({ &frontBeObserverCalcArr ,&beObserverCalcArr ,&moveRangeArr }, finalDifferenceArr);
		for (auto item = finalDifferenceArr.begin();item != finalDifferenceArr.end();item++) 
			(*item)->ChangeObserverStatus(actoViewange, 1); //设置当前对象未可视状态 

		DifferenceVector({ &beObserverCalcArr ,&frontBeObserverCalcArr ,&moveRangeArr }, finalDifferenceArr);
		for (auto item = finalDifferenceArr.begin();item != finalDifferenceArr.end();item++)
			(*item)->ChangeObserverStatus(actoViewange, 0); //设置当前对象未可视状态 
		actoViewange->ClearExtraObserverArr();
	}
}
//重计算
void AxisDistanceManager::CalcViewObj()
{
	if (m_CalcMoveList.size() <= 0) return; 
	m_AxisBodyMap->ReadyInitActorData();  
	int moveSize = m_CalcMoveList.size(); 
	ViewRange** moveArrData = m_CalcMoveList.data();  
	ViewRange* actoViewange = NULL;
	for (int i = 0;i < moveSize;i++) { //重计算玩家的信息
		actoViewange = moveArrData[i];
		actoViewange->ShiftObserverPoint();//保存上一帧数据
		m_AxisBodyMap->SetRangeActors(PointType::BODY_TYPE, PointType::BODY_TYPE, actoViewange, *actoViewange->m_ObserverPoint, actoViewange->GetViewAABB());//重新计算本帧观察
		actoViewange->ShiftBeObserverPoint();//保存上一帧数据
		m_AxisBodyMap->SetRangeActors(PointType::VIEW_TYPE, PointType::VIEW_TYPE, actoViewange, *actoViewange->m_BeObserverPoint, actoViewange->GetBodyAABB());//取得当前有谁观察了我
	}
	//经历了上面的步骤后,移动的对象,便重新计算了观察与被观察的数据信息.接下来对数据差集进行运算
	CalcBeObserver(m_CalcMoveList);
	CalcObserver(m_CalcMoveList);
	//m_CalcMoveList.clear();//不清除,节约性能
}
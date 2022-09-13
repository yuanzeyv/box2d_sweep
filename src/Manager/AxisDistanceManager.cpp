#include "Manager/AxisDistanceManager.h" 
using namespace std;    
ViewRange::ViewRange(BodyData* bodyData, b2Vec2 range) :
	m_ObserverRange(range),
	m_BodyPos(MAX_DISTANCE, MAX_DISTANCE),
	m_Actor(bodyData), 
	m_OffsetCondtion(-9999,-9999){
} 

AxisMap::~AxisMap() {
	for (int type = 0; type < PointAxisType::MAX_AXIS_TYPE; type++) {
		for (int i = 0;i < m_RecordArray[type].size();i++) {
			m_InPosActorsGenerate.BackObj(m_RecordArray[type][i]);
		}
		for (auto item = m_AxisActors[type].begin();item != m_AxisActors[type].end();item++) {
			m_b2VecIdleGenerate.BackObj(item->first);
		}
		m_AxisActors[type].clear();
		m_RecordArray[type].resize(0);
	}
}

bool AxisMap::AddtionViewRange(ViewRange* actor, b2AABB& aabb)
{
	b2Vec2* point[PointAxisType::MAX_AXIS_TYPE];
	point[PointAxisType::LEFT_BUTTOM] = &aabb.lowerBound;
	point[PointAxisType::RIGHT_TOP] = &aabb.upperBound;
	ActorID actorId = actor->GetActor()->ID();
	for (int i = 0; i < PointAxisType::MAX_AXIS_TYPE; i++)
	{
		if (m_AxisActors[i].count(point[i]) == 0) {
			auto b2Vec2Key = m_b2VecIdleGenerate.RequireObj();
			memcpy(b2Vec2Key, point[i], sizeof(b2Vec2));
			m_AxisActors[i][b2Vec2Key] = skiplist_new();
		}
		auto jumpList = m_AxisActors[i][point[i]];
		if (!skiplist_search(jumpList, actorId)){
			skiplist_insert(jumpList, actorId,(int)actor);
		} 
	}
	return true;
}
void AxisMap::DeleteViewRange(ViewRange* actor, b2AABB& aabb)
{
	b2Vec2* point[PointAxisType::MAX_AXIS_TYPE];
	point[PointAxisType::LEFT_BUTTOM] = &aabb.lowerBound;
	point[PointAxisType::RIGHT_TOP] = &aabb.upperBound;
	ActorID actorId = actor->GetActor()->ID();
	for (int i = 0; i < PointAxisType::MAX_AXIS_TYPE; i++)
	{
		if (m_AxisActors[i].count(point[i]) == 0)
			continue;
		auto jumpList = m_AxisActors[i][point[i]];
		skiplist_remove(jumpList, actorId);
		if (jumpList->count == 0) {
			delete jumpList;
			m_b2VecIdleGenerate.BackObj(m_AxisActors[i].find(point[i])->first);
			m_AxisActors[i].erase(point[i]);
		} 
	}
}
//准备计算了,初始化当前的数据信息
void AxisMap::ReadyInitActorData()
{ 
	for (int i = 0; i < PointAxisType::MAX_AXIS_TYPE; i++) {
		size_t size = m_AxisActors[i].size();//获取到当前的大小
		size_t nowSize = m_RecordArray[i].size();//获取到当前的大小
		bool isAdd = (size - nowSize) > 0;
		InPosActors** actorData = m_RecordArray[i].data();
		if (isAdd) {
			m_RecordArray[i].resize(size);
			for (int index = nowSize;index < size;index++)
				actorData[index] = m_InPosActorsGenerate.RequireObj();
		}
		else {
			for (int index = nowSize;index < size;index++)
				m_InPosActorsGenerate.BackObj(actorData[index]);
			m_RecordArray[i].resize(size);
		}
	}
	for (int i = 0; i < PointAxisType::MAX_AXIS_TYPE; i++) {
		int index = 0;
		auto& actorMap = m_AxisActors[i];
		InPosActors** actorData = m_RecordArray[i].data();//重新设置数组的大小
		for (auto actorSet = actorMap.begin(); actorSet != actorMap.end(); actorSet++) {
			actorData[index]->m_Position.Set(*actorSet->first);
			actorData[index]->m_SkipList = actorSet->second;
		}
	}
}
// 获取到范围内的所有合适的角色
int AxisMap::GetRangeActors(std::vector<ActorID>& outData, const b2AABB& range)
{
	std::vector<InPosActors*> compareArr;//当前的数据信息
	InPosActors posActor;
	posActor.m_Position = range.upperBound;
	auto lowPointItor = std::lower_bound(m_RecordArray[PointAxisType::LEFT_BUTTOM].begin(), m_RecordArray[PointAxisType::LEFT_BUTTOM].end(), &posActor,[](const InPosActors * id1, const InPosActors * id2)->bool{
		if (id1->m_Position.x != id2->m_Position.x)
			return id1->m_Position.x > id2->m_Position.x;
		return id1->m_Position.y > id2->m_Position.y;
	}); //++
	posActor.m_Position = range.lowerBound;
	auto upPointItor = std::upper_bound(m_RecordArray[PointAxisType::RIGHT_TOP].begin(), m_RecordArray[PointAxisType::RIGHT_TOP].end(), &posActor, [](const InPosActors* id1, const InPosActors* id2)->bool {
		if (id1->m_Position.x != id2->m_Position.x)
			return id1->m_Position.x > id2->m_Position.x;
		return id1->m_Position.y > id2->m_Position.y;
		});//-- 如果有相同的,直接开始迭代,否则减减后开始迭代 
	cout << m_RecordArray[PointAxisType::LEFT_BUTTOM].end() - lowPointItor << endl;
	std::copy(lowPointItor, m_RecordArray[PointAxisType::LEFT_BUTTOM].end(), compareArr.begin());//从头开始赋值
	auto midItor = compareArr.end() - (upPointItor - m_RecordArray[PointAxisType::RIGHT_TOP].begin());//获取到前的中间值
	std::copy(m_RecordArray[PointAxisType::RIGHT_TOP].begin(), upPointItor, midItor);//从头开始赋值

	

	std::inplace_merge(compareArr.begin(), midItor, compareArr.end(), InPosActors());
	outData.resize(compareArr.size());
	ActorID* actorData = outData.data();
	for (int i = 0;i < compareArr.size();i++)
		actorData[i] = compareArr[i]->m_ActorID;
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
		//开始删除角色上一帧的信息
		m_AxisBodyMap.DeleteViewRange(actorView, actorView->GetBodyAABB());
		actorView->RecalcBodyAABB();
		actorView->RecalcViewAABB();
		m_AxisBodyMap.AddtionViewRange(actorView, actorView->GetBodyAABB());
		m_CalcMoveList.push_back(item->second);
	}  
	m_DelayCalcMoveList.clear(); 
} 
 
//重计算
void AxisDistanceManager::CalcViewObj()
{
	//准备计算数据
	m_AxisBodyMap.ReadyInitActorData();
	//重新计算自己所见所得
	ViewRange* actoViewange = NULL;
	for (auto item = m_CalcMoveList.begin(); item != m_CalcMoveList.end(); item++) { 
		ActorID actorID = (*item)->GetActor()->ID();
		actoViewange = *item; 
		m_AxisBodyMap.GetRangeActors(actoViewange->GetObserverArray(), actoViewange->GetViewAABB()); 
	}
	m_CalcMoveList.clear();
} 
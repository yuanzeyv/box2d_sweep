#include "Manager/AxisDistanceManager.h" 
#include<queue>
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
			skiplist_insert(jumpList, actorId,(int64_t)actor);
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
//׼��������,��ʼ����ǰ��������Ϣ
void AxisMap::ReadyInitActorData()
{ 
	for (int i = 0; i < PointAxisType::MAX_AXIS_TYPE; i++) {
		size_t size = m_AxisActors[i].size();//��ȡ����ǰ�Ĵ�С
		size_t nowSize = m_RecordArray[i].size();//��ȡ����ǰ�Ĵ�С
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
		InPosActors** actorData = m_RecordArray[i].data();//������������Ĵ�С
		for (auto actorSet = actorMap.begin(); actorSet != actorMap.end(); actorSet++) {
			actorData[index]->m_Position.Set(*actorSet->first);
			actorData[index]->m_SkipList = actorSet->second;
			index++;
		}
	}
}//���������������
struct node {
	sk_link* m_Point;  //  Ԫ��ֵ
	ViewRange* m_ViewRange;  //  Ԫ��ֵ
	int k;   //Ԫ����������
	int i;  // Ԫ�����������±�
};
struct compare
{
	bool operator()(node& node1, node& node2)
	{
		if (node1.i == INT_MAX || node2.i == INT_MAX)
			return node1.i > node2.i;
		return node1.m_ViewRange->GetActor()->ID() > node2.m_ViewRange->GetActor()->ID();//����С����
	}
};
void mul_sort(std::vector<InPosActors*>& nums, vector<ViewRange*>& res) {
	int size = 0;
	for (auto item = nums.begin(); item != nums.end(); item++)
		size += (*item)->m_SkipList->count;
	res.resize(size);

	int k = nums.size();
	priority_queue<node, vector<node>, compare> que;
	for (int i = 0; i < k; i++) { 
		struct sk_link* pos = nums[i]->m_SkipList->head[0].next; 
		struct skipnode* node = list_entry(pos, struct skipnode, link[0]);
		ViewRange* range = (ViewRange*)node->value;
		que.push({ pos,range,i,0 });
	} 
	for (int j = 0; j < size; j++) {
		node currt = que.top();
		que.pop();
		res[j] = currt.m_ViewRange;
		node next;
		next.i = currt.i + 1;
		next.k = currt.k;
		if (next.i >= nums[currt.k]->m_SkipList->count) {
			next.i = INT_MAX; 
		}
		else {
			next.m_Point = currt.m_Point->next;
			next.m_ViewRange = (ViewRange*)list_entry(next.m_Point, struct skipnode, link[0])->value;
		}
		que.push(next);
	}

}  
// ��ȡ����Χ�ڵ����к��ʵĽ�ɫ
int AxisMap::GetRangeActors(std::vector<ViewRange*>& outData, const b2AABB& range)
{
	std::vector<InPosActors*> compareArr;//��ǰ��������Ϣ
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
		});//-- �������ͬ��,ֱ�ӿ�ʼ����,���������ʼ���� 

	compareArr.resize((m_RecordArray[PointAxisType::LEFT_BUTTOM].end() - lowPointItor) + (upPointItor  - m_RecordArray[PointAxisType::RIGHT_TOP].begin()) );
	std::copy(lowPointItor, m_RecordArray[PointAxisType::LEFT_BUTTOM].end(), compareArr.begin());//��ͷ��ʼ��ֵ
	auto midItor = compareArr.end() - (upPointItor - m_RecordArray[PointAxisType::RIGHT_TOP].begin());//��ȡ��ǰ���м�ֵ
	std::copy(m_RecordArray[PointAxisType::RIGHT_TOP].begin(), upPointItor, midItor);//��ͷ��ʼ��ֵ 

	mul_sort(compareArr,outData);
	outData.resize( unique(outData.begin(), outData.end())- outData.begin());
	return outData.size();
}


void AxisDistanceManager::ActorsMove()
{
	ViewRange* actorView = NULL;//�����ƶ����еĽ�ɫ
	bool isDelete = false;//��ǰ�Ƿ��������  
	auto iterEnd = m_DelayCalcMoveList.end();//��ֹ�ظ���ȡ��end
	for (auto item = m_DelayCalcMoveList.begin(); item != iterEnd;item++) { //ѭ�����������е��ƶ��Ľ�ɫ
		ActorID actorID = item->first; //��ȡ����ɫID
		actorView = item->second;//��ȡ����ɫ����ͼ����
		if(actorView->IsRefreshActorView() == 0)//�ƶ������ǳ
			continue;
		actorView->RecalcBodyPosition();//�ؼ����ɫ��λ��  
		//��ʼɾ����ɫ��һ֡����Ϣ
		m_AxisBodyMap.DeleteViewRange(actorView, actorView->GetBodyAABB());
		actorView->RecalcBodyAABB();
		actorView->RecalcViewAABB();
		m_AxisBodyMap.AddtionViewRange(actorView, actorView->GetBodyAABB());
		m_CalcMoveList.push_back(item->second);
	}  
	m_DelayCalcMoveList.clear(); 
} 
 
//�ؼ���
void AxisDistanceManager::CalcViewObj()
{
	//׼����������
	m_AxisBodyMap.ReadyInitActorData();
	//���¼����Լ���������
	ViewRange* actoViewange = NULL;
	for (auto item = m_CalcMoveList.begin(); item != m_CalcMoveList.end(); item++) { 
		ActorID actorID = (*item)->GetActor()->ID();
		actoViewange = *item; 
		m_AxisBodyMap.GetRangeActors(actoViewange->GetObserverArray(), actoViewange->GetViewAABB()); 
	}
	m_CalcMoveList.clear();
} 
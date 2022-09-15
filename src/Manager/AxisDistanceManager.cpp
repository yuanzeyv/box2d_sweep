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
	m_FrontObserverPoint = &m_ObserverArr[0];//��һ��ԭʼ������һ֡���Կ�����
	m_ObserverPoint = &m_ObserverArr[1];//��һ��ԭʼ������һ֡���Կ����� 
	m_FrontBeObserverPoint = &m_BeObserverArr[0];//��һ��ԭʼ������һ֡���Կ�����
	m_BeObserverPoint = &m_BeObserverArr[1];//��һ��ԭʼ������һ֡���Կ����� 
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
void ViewRange::ClearExtraObserverArr()//ɾ�����ж���Ĺ۲����
{
	for (auto item = m_ExtraObserverArr.begin();item != m_ExtraObserverArr.end();item++) {
		m_ViewObserverStateGenerate.BackObj(item->second);
	}
	m_ExtraObserverArr.clear();
}

void ViewRange::ClearExtraBeObserverArr()//ɾ�����ж���Ĺ۲����
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
	float lowX = aabb.lowerBound.x;//����͵��X 
	ActorID actorId = actor->GetActor()->ID();
	if (m_XAxisActors[type].count(lowX) == 0)
		m_XAxisActors[type][lowX] = skiplist_new();
	auto jumpList = m_XAxisActors[type][lowX];
	if (!skiplist_search(jumpList, actorId)){//����
		skiplist_insert(jumpList, actorId,(int64_t)actor);
	}  
}
 
void AxisMap::DeleteViewRange(PointType type,ViewRange* actor, b2AABB& aabb)
{
	float lowX = aabb.lowerBound.x;//����͵��X   
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
//׼��������,��ʼ����ǰ��������Ϣ
void AxisMap::ReadyInitActorData()
{ 
	for (int i = 0; i < PointType::MAX_POINT_TYPE;i++) {
		int index = 0;
		size_t size = m_XAxisActors[i].size();//��ȡ����ǰ�Ĵ�С
		size_t nowSize = m_RecordArray[i].size();//��ȡ����ǰ�Ĵ�С
		if (size != nowSize) {
			if ((size - nowSize) > 0) {//��Ҫ��ӵ����
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
		InPosActors** actorData = m_RecordArray[i].data();//������������Ĵ�С
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
// ��ȡ����Χ�ڵ����к��ʵĽ�ɫ
int AxisMap::SetRangeActors(PointType type, ViewRange* actor,std::vector<ViewObserverState*>& outData, const b2AABB& range)
{ 
	InPosActors posActor; 
	posActor.m_PositionX = range.upperBound.x; 
	auto lowPointItorSize = std::lower_bound(m_RecordArray[type].begin(), m_RecordArray[type].end(), &posActor, InPosActors()) - m_RecordArray[type].begin(); //���ж���X,���ж�һ��Y
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
	sort(tempData.begin(), tempData.end(), ViewRangeIDCompare); //����֮��,������� 
	size_t tempSize = tempData.size();//��ȡ����ǰ�Ĵ�С
	size_t nowSize = outData.size();//��ȡ����ǰ�Ĵ�С
	if (nowSize < tempSize)
		outData.resize(tempSize);//��������
	ViewObserverState** outDataArr = outData.data();
	size_t loopCount = max(tempSize, nowSize);
	for (int i = 0; i < loopCount;i++) {
		if (i < tempSize) {//���õĻ�
			if (!outDataArr[i])//�˻�
				outDataArr[i] = actor->m_ViewObserverStateGenerate.RequireObj();
			outDataArr[i]->m_IsVisible = 0;//�ɼ�
			outDataArr[i]->m_ViewRange = tempDataArr[i];
		} else {
			if(outDataArr[i])//�˻�
				actor->m_ViewObserverStateGenerate.BackObj(outDataArr[i]);
		} 
	}
	if (nowSize > tempSize)
		outData.resize(tempSize);//�������� 
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
void AxisDistanceManager::CalcObserver(vector<ViewRange*>& moveRangeArr)//�ؼ��� 
{
	//ȡ�ö�����һ֡���۲�ĵ������е���Ϣ
	vector<ViewRange*> frontObserverCalcArr;
	//ȡ�ö�����һ֡���۲�ĵ������е���Ϣ
	vector<ViewRange*> observerCalcArr; 
	vector<ViewRange*> differenceInVisibleCalcArr;//ͨ����ɼ��б�ȡ�,�޳��ޱ仯����Ա��Ϣ
	vector<ViewRange*> differenceVisibleCalcArr;//ͨ����ɼ��б�ȡ�,�޳��ޱ仯����Ա��Ϣ 
	vector<ViewRange*> finalDifferenceArr;//ͨ����ɼ��б�ȡ�,�޳��ޱ仯����Ա��Ϣ 
	int moveSize = moveRangeArr.size(); 
	ViewRange** moveArrData = moveRangeArr.data();
	ViewRange* actoViewange = NULL;
	ViewRange** observerArr = NULL;
	int index;
	for (int i = 0;i < moveSize;i++) { 
		actoViewange = moveArrData[i];
		frontObserverCalcArr.resize(actoViewange->m_FrontObserverPoint->size() + actoViewange->m_ExtraObserverArr.size());
		//ȡ�ö�����һ֡���۲�ĵ������е���Ϣ
		observerArr = frontObserverCalcArr.data();
		index = 0;
		for (int pos = 0;pos < actoViewange->m_FrontObserverPoint->size();pos++) {
			if (actoViewange->m_FrontObserverPoint->at(pos)->m_IsVisible == 1)
				continue;
			observerArr[index++] = actoViewange->m_FrontObserverPoint->at(pos)->m_ViewRange;//�ɼ��ż���,������Ϊ��ɾ����
		}
		for (auto item = actoViewange->m_ExtraObserverArr.begin();item != actoViewange->m_ExtraObserverArr.end();item++) {
			if (item->second->m_IsVisible == 1)
				continue;
			observerArr[index++] = item->second->m_ViewRange;//�ɼ��ż���,������Ϊ��ɾ����
		}
		frontObserverCalcArr.resize(index);
		//ȡ�ö�����һ֡���۲�ĵ������е���Ϣ 
		observerCalcArr.resize(actoViewange->m_ObserverPoint->size());
		observerArr = observerCalcArr.data();
		for (int pos = 0;pos < actoViewange->m_ObserverPoint->size();pos++) {
			observerArr[pos] = actoViewange->m_ObserverPoint->at(pos)->m_ViewRange;
		}
		//ǰһ֡ �� ��֡ȡ�. �õ�Ӧ�ñ�����Ϊ���صĵ�Ԫ
		set_difference(frontObserverCalcArr.begin(), frontObserverCalcArr.end(), observerCalcArr.begin(), observerCalcArr.end(), inserter(differenceInVisibleCalcArr, differenceInVisibleCalcArr.begin()), ViewRangePointComapre);
		set_difference(differenceInVisibleCalcArr.begin(), differenceInVisibleCalcArr.end(), moveRangeArr.begin(), moveRangeArr.end(), inserter(finalDifferenceArr, finalDifferenceArr.begin()), ViewRangePointComapre);
		for (auto item = finalDifferenceArr.begin();item != finalDifferenceArr.end();item++) {
			actoViewange->ChangeBeObserverStatus(*item, 1); //���õ�ǰ����δ����״̬
		}
		set_difference(observerCalcArr.begin(), observerCalcArr.end(), frontObserverCalcArr.begin(), frontObserverCalcArr.end(), inserter(differenceVisibleCalcArr, differenceVisibleCalcArr.begin()), ViewRangePointComapre);
		set_difference(differenceVisibleCalcArr.begin(), differenceVisibleCalcArr.end(), moveRangeArr.begin(), moveRangeArr.end(), inserter(finalDifferenceArr, finalDifferenceArr.begin()), ViewRangePointComapre);
		for (auto item = finalDifferenceArr.begin();item != finalDifferenceArr.end();item++) {
			actoViewange->ChangeBeObserverStatus(*item, 0); //���õ�ǰ����δ����״̬
		}
		observerCalcArr.clear();
		differenceInVisibleCalcArr.clear();
		differenceVisibleCalcArr.clear();
		finalDifferenceArr.clear();
		actoViewange->ClearExtraBeObserverArr();
	}
}

void AxisDistanceManager::CalcBeObserver(vector<ViewRange*>& moveRangeArr)//�ؼ��� 
{
	//ȡ�ö�����һ֡���۲�ĵ������е���Ϣ
	vector<ViewRange*> frontBeObserverCalcArr;
	//ȡ�ö�����һ֡���۲�ĵ������е���Ϣ
	vector<ViewRange*> BeObserverCalcArr;
	vector<ViewRange*> differenceInVisibleCalcArr;//ͨ����ɼ��б�ȡ�,�޳��ޱ仯����Ա��Ϣ
	vector<ViewRange*> differenceVisibleCalcArr;//ͨ����ɼ��б�ȡ�,�޳��ޱ仯����Ա��Ϣ 
	vector<ViewRange*> finalDifferenceArr;//ͨ����ɼ��б�ȡ�,�޳��ޱ仯����Ա��Ϣ 
	int moveSize = moveRangeArr.size();
	ViewRange** moveArrData = moveRangeArr.data();
	ViewRange* actoViewange = NULL;
	ViewRange** observerArr = NULL;
	int index;
	for (int i = 0;i < moveSize;i++) {
		actoViewange = moveArrData[i];
		frontBeObserverCalcArr.resize(actoViewange->m_FrontObserverPoint->size() + actoViewange->m_ExtraObserverArr.size());
		//ȡ�ö�����һ֡���۲�ĵ������е���Ϣ
		observerArr = frontBeObserverCalcArr.data();
		index = 0;
		for (int pos = 0;pos < actoViewange->m_FrontObserverPoint->size();pos++) {
			if (actoViewange->m_FrontObserverPoint->at(pos)->m_IsVisible == 1)
				continue;
			observerArr[index++] = actoViewange->m_FrontObserverPoint->at(pos)->m_ViewRange;//�ɼ��ż���,������Ϊ��ɾ����
		}
		for (auto item = actoViewange->m_ExtraObserverArr.begin();item != actoViewange->m_ExtraObserverArr.end();item++) {
			if (item->second->m_IsVisible == 1)
				continue;
			observerArr[index++] = item->second->m_ViewRange;//�ɼ��ż���,������Ϊ��ɾ����
		}
		frontBeObserverCalcArr.resize(index);
		//ȡ�ö�����һ֡���۲�ĵ������е���Ϣ 
		BeObserverCalcArr.resize(actoViewange->m_ObserverPoint->size());
		observerArr = BeObserverCalcArr.data();
		for (int pos = 0;pos < actoViewange->m_ObserverPoint->size();pos++) {
			observerArr[pos] = actoViewange->m_ObserverPoint->at(pos)->m_ViewRange;
		}
		//ǰһ֡ �� ��֡ȡ�. �õ�Ӧ�ñ�����Ϊ���صĵ�Ԫ
		set_difference(frontBeObserverCalcArr.begin(), frontBeObserverCalcArr.end(), BeObserverCalcArr.begin(), BeObserverCalcArr.end(), inserter(differenceInVisibleCalcArr, differenceInVisibleCalcArr.begin()), ViewRangePointComapre);
		set_difference(differenceInVisibleCalcArr.begin(), differenceInVisibleCalcArr.end(), moveRangeArr.begin(), moveRangeArr.end(), inserter(finalDifferenceArr, finalDifferenceArr.begin()), ViewRangePointComapre);
		for (auto item = finalDifferenceArr.begin();item != finalDifferenceArr.end();item++) {
			actoViewange->ChangeObserverStatus(*item, 1); //���õ�ǰ����δ����״̬
		} 
		set_difference(BeObserverCalcArr.begin(), BeObserverCalcArr.end(), frontBeObserverCalcArr.begin(), frontBeObserverCalcArr.end(), inserter(differenceVisibleCalcArr, differenceVisibleCalcArr.begin()), ViewRangePointComapre);
		set_difference(differenceVisibleCalcArr.begin(), differenceVisibleCalcArr.end(), moveRangeArr.begin(), moveRangeArr.end(), inserter(finalDifferenceArr, finalDifferenceArr.begin()), ViewRangePointComapre);
		for (auto item = finalDifferenceArr.begin();item != finalDifferenceArr.end();item++) {
			actoViewange->ChangeObserverStatus(*item, 0); //���õ�ǰ����δ����״̬
		}
		BeObserverCalcArr.clear();
		differenceInVisibleCalcArr.clear();
		differenceVisibleCalcArr.clear();
		finalDifferenceArr.clear();
		actoViewange->ClearExtraObserverArr();
	}
}
//�ؼ���
void AxisDistanceManager::CalcViewObj()
{
	//׼����������
	m_AxisBodyMap->ReadyInitActorData();
	//��ȡ����ǰ���ƶ��б������
	int index = 0;
	int moveSize = m_CalcMoveList.size();
	vector<ViewRange*> moveArr(moveSize);
	ViewRange** moveArrData = moveArr.data();
	for (auto item = m_CalcMoveList.begin();item != m_CalcMoveList.end();item++)
		moveArrData[index++] = item->second;  
	//�����������е���ҵ���ҰӢ��
	ViewRange* actoViewange = NULL;
	for (int i = 0;i < moveSize;i++) { 
		moveArrData[i]->ShiftObserverPoint();//������һ֡����
		m_AxisBodyMap->SetRangeActors(PointType::BODY_TYPE, moveArrData[i], *moveArrData[i]->m_ObserverPoint, moveArrData[i]->GetViewAABB());//���¼��㱾֡�۲�
		moveArrData[i]->ShiftBeObserverPoint();//������һ֡����
		m_AxisBodyMap->SetRangeActors(PointType::VIEW_TYPE, moveArrData[i], *moveArrData[i]->m_BeObserverPoint, moveArrData[i]->GetBodyAABB());//ȡ�õ�ǰ��˭�۲�����
	}
	CalcBeObserver(moveArr);
	CalcObserver(moveArr);
	m_CalcMoveList.clear();   
}
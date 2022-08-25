#include "Navmesh/Navmesh.h"   
#include "Detour/DetourCommon.h"
#include "Detour/DetourNavMeshBuilder.h"
using namespace std; 
//对每个地图构建一个ID
//支持多个地图构建
//每个地图绑定多个角色
//每个地图有一个全局的直接的地图索引
//每个角色可以添加一个切片寻路索引
//针对射线,尽量保证距离够短
static inline bool inRange(const float* v1, const float* v2, const float r, const float h)
{
	const float dx = v2[0] - v1[0];
	const float dy = v2[1] - v1[1];
	const float dz = v2[2] - v1[2];
	return (dx * dx + dz * dz) < r * r && fabsf(dy) < h;
} 
// Returns a random number [0..1]
static float frand()
{ 
	return (float)rand() / (float)RAND_MAX;
}

static bool getSteerTarget(dtNavMeshQuery* navQuery, const float* startPos, const float* endPos,const float minTargetDist,const dtPolyRef* path,
	const int pathSize,float* steerPos, unsigned char& steerPosFlag, dtPolyRef& steerPosRef,float* outPoints = 0, int* outPointCount = 0)
{
	// Find steer target.
	static const int MAX_STEER_POINTS = 3;
	float steerPath[MAX_STEER_POINTS * 3];
	unsigned char steerPathFlags[MAX_STEER_POINTS];
	dtPolyRef steerPathPolys[MAX_STEER_POINTS];
	int nsteerPath = 0;
	navQuery->findStraightPath(startPos, endPos, path, pathSize,
		steerPath, steerPathFlags, steerPathPolys, &nsteerPath, MAX_STEER_POINTS);
	if (!nsteerPath)
		return false; 
	if (outPoints && outPointCount)
	{
		*outPointCount = nsteerPath;
		for (int i = 0; i < nsteerPath; ++i)
			dtVcopy(&outPoints[i * 3], &steerPath[i * 3]);
	}  
	// Find vertex far enough to steer to.
	int ns = 0;
	while (ns < nsteerPath)
	{
		// Stop at Off-Mesh link or when point is further than slop away.
		if ((steerPathFlags[ns] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ||
			!inRange(&steerPath[ns * 3], startPos, minTargetDist, 1000.0f))
			break;
		ns++;
	}
	// Failed to find good point to steer to.
	if (ns >= nsteerPath)
		return false; 
	dtVcopy(steerPos, &steerPath[ns * 3]);
	steerPos[1] = startPos[1];
	steerPosFlag = steerPathFlags[ns];
	steerPosRef = steerPathPolys[ns]; 
	return true;
}
static int fixupCorridor(dtPolyRef* path, const int npath, const int maxPath,const dtPolyRef* visited, const int nvisited)
{
	int furthestPath = -1;
	int furthestVisited = -1;
	// Find furthest common polygon.
	for (int i = npath - 1; i >= 0; --i)
	{
		bool found = false;
		for (int j = nvisited - 1; j >= 0; --j)
		{
			if (path[i] == visited[j])
			{
				furthestPath = i;
				furthestVisited = j;
				found = true;
			}
		}
		if (found)
			break;
	}
	// If no intersection found just return current path. 
	if (furthestPath == -1 || furthestVisited == -1)
		return npath;
	// Concatenate paths.	
	// Adjust beginning of the buffer to include the visited.
	const int req = nvisited - furthestVisited;
	const int orig = rcMin(furthestPath + 1, npath);
	int size = rcMax(0, npath - orig);
	if (req + size > maxPath)
		size = maxPath - req;
	if (size)
		memmove(path + req, path + orig, size * sizeof(dtPolyRef));
	// Store visited
	for (int i = 0; i < req; ++i)
		path[i] = visited[(nvisited - 1) - i];
	return req + size;
}

static int fixupShortcuts(dtPolyRef* path, int npath, dtNavMeshQuery* navQuery)
{
	if (npath < 3)
		return npath;
	// Get connected polygons
	static const int maxNeis = 16;
	dtPolyRef neis[maxNeis];
	int nneis = 0;
	const dtMeshTile* tile = 0;
	const dtPoly* poly = 0;
	if (dtStatusFailed(navQuery->getAttachedNavMesh()->getTileAndPolyByRef(path[0], &tile, &poly)))
		return npath;
	for (unsigned int k = poly->firstLink; k != DT_NULL_LINK; k = tile->links[k].next)
	{
		const dtLink* link = &tile->links[k];
		if (link->ref != 0)
		{
			if (nneis < maxNeis)
				neis[nneis++] = link->ref;
		}
	}
	// If any of the neighbour polygons is within the next few polygons
	// in the path, short cut to that polygon directly.
	static const int maxLookAhead = 6;
	int cut = 0;
	for (int i = dtMin(maxLookAhead, npath) - 1; i > 1 && cut == 0; i--) {
		for (int j = 0; j < nneis; j++)
		{
			if (path[i] == neis[j]) {
				cut = i;
				break;
			}
		}
	}
	if (cut > 1)
	{
		int offset = cut - 1;
		npath -= offset;
		for (int i = 1; i < npath; i++)
			path[i] = path[i + offset];
	}
	return npath;
}
FindWayFalt::FindWayFalt()
{
	PathPolysCount = 0;
	MeshQuery = NULL;
	PathFindStatus = DT_SUCCESS;
}
FindWayFalt::FindWayFalt(dtNavMesh* navmesh)
{
	FindWayFalt();
	SetNavMesh(navmesh);
}

FindWayFalt::~FindWayFalt()
{
	dtFreeNavMeshQuery(MeshQuery);
} 



bool Navmesh::GenerateNavMesh(string path)
{
	if (!NavmeshBuild->LoadGeometry(path))
		return false;
	if (!NavmeshBuild->BuildMap())
		return false;
	this->NavMeshCell = NavmeshBuild->NavMesh;
	this->NavQueryCell.MeshQuery = NavmeshBuild->GeneratorNavMeshQuery();
	return true;
}
 
float Navmesh::Recast(PosV3& startPos, PosV3& endPos)
{
	float hitPercent = 0;
	NavQueryCell.StartPos = startPos;
	NavQueryCell.EndPos = endPos;
	 
	NavQueryCell.MeshQuery->findNearestPoly(startPos.Addr(), PolyPickExt.Addr(),&Filter, &NavQueryCell.StartRef, 0);
	NavQueryCell.MeshQuery->findNearestPoly(endPos.Addr(), PolyPickExt.Addr(),&Filter, &NavQueryCell.EndRef, 0);
	if (!(NavQueryCell.StartRef && NavQueryCell.EndRef))
		return hitPercent;
	NavQueryCell.MeshQuery->raycast(NavQueryCell.StartRef, startPos.Addr(), endPos.Addr(), &Filter, &hitPercent, PosV3(0,0,0).Addr(), NavQueryCell.PathPolys, &NavQueryCell.PathPolysCount, MAX_POLYS);
	if (hitPercent > 1) {
		hitPercent = 1;
	}
	return hitPercent;
}
bool Navmesh::SoomthPaht()//将当前路径转换为平滑路径
{
	SmoothPathCount = 0;
	if (!NavQueryCell.PathPolysCount) {//如果当前有寻找到制定的路径的话
		return false;
	}
	// Iterate over the path to find smooth path on the detail mesh surface.
	dtPolyRef tempPolys[MAX_POLYS];//获取到一个临时的
	int polysCount = NavQueryCell.PathPolysCount;//获取到一个零时的数目
	memcpy(tempPolys, NavQueryCell.PathPolys, sizeof(dtPolyRef) * NavQueryCell.PathPolysCount);//进行复制拷贝,之后就不使用了

	float iterPos[3], targetPos[3];
	NavQueryCell.MeshQuery->closestPointOnPoly(tempPolys[0], NavQueryCell.StartPos.Addr(), iterPos, 0);//找到高度
	NavQueryCell.MeshQuery->closestPointOnPoly(tempPolys[polysCount - 1], NavQueryCell.EndPos.Addr(), targetPos, 0);//找到高度
	static const float STEP_SIZE = 0.5f;//步长
	static const float SLOP = 0.01f;//溢出 
	dtVcopy(&SmoothPathArray[SmoothPathCount * 3], iterPos);//头copy
	SmoothPathCount++;//添加一个点 
	// Move towards target a small advancement at a time until target reached or
	// when ran out of memory to store the path.
	while (polysCount && SmoothPathCount < MAX_SMOOTH)
	{
		// Find location to steer towards.
		float steerPos[3];
		unsigned char steerPosFlag;
		dtPolyRef steerPosRef; 
		if (!getSteerTarget(NavQueryCell.MeshQuery, iterPos, targetPos, SLOP,tempPolys, polysCount, steerPos, steerPosFlag, steerPosRef))
			break; 
		bool endOfPath = (steerPosFlag & DT_STRAIGHTPATH_END) ? true : false;
		bool offMeshConnection = (steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ? true : false; 
		// Find movement delta.
		float delta[3], len;
		dtVsub(delta, steerPos, iterPos);
		len = dtMathSqrtf(dtVdot(delta, delta));
		// If the steer target is end of path or off-mesh link, do not move past the location.
		if ((endOfPath || offMeshConnection) && len < STEP_SIZE)
			len = 1;
		else
			len = STEP_SIZE / len;
		float moveTgt[3];
		dtVmad(moveTgt, iterPos, delta, len);  
		// Move
		float result[3];
		dtPolyRef visited[16];
		int nvisited = 0;
		NavQueryCell.MeshQuery->moveAlongSurface(tempPolys[0], iterPos, moveTgt, &Filter,result, visited, &nvisited, 16);

		polysCount = fixupCorridor(tempPolys, polysCount, MAX_POLYS, visited, nvisited);
		polysCount = fixupShortcuts(tempPolys, polysCount, NavQueryCell.MeshQuery);

		float h = 0;
		NavQueryCell.MeshQuery->getPolyHeight(tempPolys[0], result, &h);
		result[1] = h;
		dtVcopy(iterPos, result);

		// Handle end of path and off-mesh links when close enough.
		if (endOfPath && inRange(iterPos, steerPos, SLOP, 1.0f))
		{
			// Reached end of path.
			dtVcopy(iterPos, targetPos);
			if (SmoothPathCount < MAX_SMOOTH)
			{
				dtVcopy(&SmoothPathArray[SmoothPathCount * 3], iterPos);
				SmoothPathCount++;
			}
			break;
		}
		else if (offMeshConnection && inRange(iterPos, steerPos, SLOP, 1.0f))
		{
			// Reached off-mesh connection.
			float startPos[3], endPos[3];

			// Advance the path up to and over the off-mesh connection.
			dtPolyRef prevRef = 0, polyRef = tempPolys[0];
			int npos = 0;
			while (npos < polysCount && polyRef != steerPosRef)
			{
				prevRef = polyRef;
				polyRef = tempPolys[npos];
				npos++;
			}
			for (int i = npos; i < polysCount; ++i)
				tempPolys[i - npos] = tempPolys[i];
			polysCount -= npos;

			// Handle the connection.
			dtStatus status = NavMeshCell->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, startPos, endPos);
			if (dtStatusSucceed(status))
			{
				if (SmoothPathCount < MAX_SMOOTH)
				{
					dtVcopy(&SmoothPathArray[SmoothPathCount * 3], startPos);
					SmoothPathCount++;
					// Hack to make the dotted path not visible during off-mesh connection.
					if (SmoothPathCount & 1)
					{
						dtVcopy(&SmoothPathArray[SmoothPathCount * 3], startPos);
						SmoothPathCount++;
					}
				}
				// Move position at the other side of the off-mesh link.
				dtVcopy(iterPos, endPos);
				float eh = 0.0f;
				NavQueryCell.MeshQuery->getPolyHeight(tempPolys[0], iterPos, &eh);
				iterPos[1] = eh;
			}
		}
		// Store results.
		if (SmoothPathCount < MAX_SMOOTH)//如果在其中的话
		{
			dtVcopy(&SmoothPathArray[SmoothPathCount * 3], iterPos);//添加
			SmoothPathCount++;
		}
	} 
	return true;
}

bool Navmesh::FindRodomPoint(PosV3& pos)
{
	dtPolyRef ref;
	NavQueryCell.MeshQuery->findRandomPoint(&Filter, frand, &ref, pos.Addr());
	return true;
}
bool Navmesh::FindPath(PosV3& startPos, PosV3& endPos,bool sommthPath )//可以选择是否输出平滑路径
{
	NavQueryCell.StartPos = startPos;
	NavQueryCell.EndPos = endPos; 

	NavQueryCell.MeshQuery->findNearestPoly(startPos.Addr(), PolyPickExt.Addr(), &Filter, &NavQueryCell.StartRef, 0);
	NavQueryCell.MeshQuery->findNearestPoly(endPos.Addr(), PolyPickExt.Addr(), &Filter, &NavQueryCell.EndRef, 0);
	if (!(NavQueryCell.StartRef && NavQueryCell.EndRef))
		return false; 
	NavQueryCell.MeshQuery->findPath(NavQueryCell.StartRef, NavQueryCell.EndRef, startPos.Addr(), endPos.Addr(), &Filter, NavQueryCell.PathPolys, &NavQueryCell.PathPolysCount, MAX_POLYS); 

	if (sommthPath)
		SoomthPaht();//获取到平滑路径
	else
		SmoothPathCount = 0;//无平滑路径,需要重置一下下标
	return true; 
}

bool Navmesh::FindPath_Sliced(ActorID actorID, PosV3& startPos, PosV3& endPos)
{
	//首先查看当前是否存在一个对于角色的查询
	if (SlicedQueryMap.find(actorID) != SlicedQueryMap.end())//已经查询过 了
		return false;
	//取一个空闲的寻路结构
	FindWayFalt* findWayFalt = GetIdleFindWayFalt();  

	findWayFalt->StartPos = startPos;
	findWayFalt->EndPos = endPos;
	findWayFalt->MeshQuery->findNearestPoly(startPos.Addr(), PolyPickExt.Addr(),&Filter, &NavQueryCell.StartRef, 0);
	findWayFalt->MeshQuery->findNearestPoly(endPos.Addr(), PolyPickExt.Addr(), &Filter, &NavQueryCell.EndRef, 0);
	findWayFalt->PathFindStatus = findWayFalt->MeshQuery->initSlicedFindPath(NavQueryCell.StartRef, NavQueryCell.EndRef, startPos.Addr(), endPos.Addr(), &Filter, DT_FINDPATH_ANY_ANGLE);

	SlicedQueryMap[actorID] = findWayFalt;//加入到循环队列 
	SlicedUnFinishTable[actorID] = findWayFalt;//添加一个  
	return true;
}

bool Navmesh::Update_FindPath_Sliced(ActorID actorID , FindWayFalt* findWayFalt)
{  
	if (dtStatusInProgress(findWayFalt->PathFindStatus))
	{
		findWayFalt->PathFindStatus = findWayFalt->MeshQuery->updateSlicedFindPath(15, 0);//迭代一次
	}
	else if (dtStatusSucceed(findWayFalt->PathFindStatus))
	{
		findWayFalt->MeshQuery->finalizeSlicedFindPath(findWayFalt->PathPolys, &findWayFalt->PathPolysCount, MAX_POLYS);//寻找完成后,依然不是最优路径 
		SlicedFinishTable[actorID] = findWayFalt;//添加一个  
		return true;
	} 
	return false;
}

//#include <chrono>
//using namespace std;
//更新函数
void Navmesh::Update(float dt)
{
	//auto t1 = chrono::high_resolution_clock::now();
	if (SlicedUnFinishTable.size())//拥有时
	for (auto item = SlicedUnFinishTable.begin(); item != SlicedUnFinishTable.end();)
		if (Update_FindPath_Sliced(item->first, item->second))
			item = SlicedUnFinishTable.erase(item);
		else
			item++;
	//auto t2 = chrono::high_resolution_clock::now();
	///int64_t  a = chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
//	printf("执行5000次射线的时间为:%lld  %d\n\r", a, SlicedFinishTable.size());
}
Navmesh::Navmesh() :PolyPickExt(2.f, 4.f, 2.f)
{  
	NavmeshBuild = new NavmeshBuilder(&Ctx);
}
Navmesh::~Navmesh()
{
	//删除正在寻路的片段
	SlicedFinishTable.clear();
	SlicedUnFinishTable.clear();
	for (auto item = SlicedQueryMap.begin(); item != SlicedQueryMap.end();item++)
		IdleQueryVector.push(item->second);
	SlicedQueryMap.clear();//清空所有的 
	//删除空闲的寻路
	while (true)
	{ 
		if (!IdleQueryVector.size())
			break;
		delete IdleQueryVector.top();
		IdleQueryVector.pop();
	}  
}
void Navmesh::BreakActorSliced(ActorID id) { 
	if (SlicedQueryMap.find(id) == SlicedQueryMap.end())
		return;
	if (SlicedUnFinishTable.find(id) != SlicedQueryMap.end())
		SlicedUnFinishTable.erase(id);
	else if(SlicedFinishTable.find(id) != SlicedQueryMap.end())
		SlicedFinishTable.erase(id); 
	IdleQueryVector.push(SlicedQueryMap.find(id)->second);
	SlicedQueryMap.erase(id); 
} 

FindWayFalt* Navmesh::GetIdleFindWayFalt()
{
	FindWayFalt* retFalt = NULL;
	if (IdleQueryVector.size() > 0) {
		retFalt = IdleQueryVector.top();
		IdleQueryVector.pop();
		return retFalt;
	}
	retFalt = new FindWayFalt();
	retFalt->MeshQuery = NavmeshBuild->GeneratorNavMeshQuery();
	return retFalt;
}
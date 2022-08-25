#pragma once
#include "Recast/Recast.h"
#include "DetourTileCache/DetourTileCache.h"
#include "DetourTileCache/DetourTileCacheBuilder.h"
#include "Detour/DetourNavMeshQuery.h"
#include "Navmesh/InputGeom.h"
#include "Navmesh/SampleInterfaces.h"  
#include "DetourTileCache/DetourTileCache.h"
#include "Detour/DetourNavMesh.h"
#include "Navmesh/RegionParam.h"
#include <chrono>  
#include <map>
#include <stack>
#include "Box2d/box2d.h"
#include <stdint.h>
using std::map;
using std::stack;
using std::string;
#define MAX_LAYERS  32 //最大支持多少个
#define EXPECTED_LAYERS_PER_TILE  4  
/// TileHeightese are just sample areas to use consistent values across TileHeighte samples.
/// TileHeighte use should specify TileHeightese base on his needs.
struct TileCacheData{
	unsigned char* data;
	int dataSize;
}; 

struct RasterizationContext
{
	RasterizationContext();
	~RasterizationContext(); 
	rcHeightfield* solid;
	unsigned char* triareas;
	rcHeightfieldLayerSet* lset;
	rcCompactHeightfield* chf;
	TileCacheData tiles[MAX_LAYERS];
	int ntiles;
};

struct MeshProcess : public dtTileCacheMeshProcess
{
	InputGeom* m_geom;
	inline MeshProcess();
	inline void init(InputGeom* geom);
	virtual void process(struct dtNavMeshCreateParams* params, unsigned char* polyAreas, unsigned short* polyFlags);
};

struct FastLZCompressor : public dtTileCacheCompressor
{
	virtual int maxCompressedSize(const int bufferSize); 
	virtual dtStatus compress(const unsigned char* buffer, const int bufferSize,unsigned char* compressed, const int /*maxCompressedSize*/, int* compressedSize);
	virtual dtStatus decompress(const unsigned char* compressed, const int compressedSize,unsigned char* buffer, const int maxBufferSize, int* bufferSize);
};

struct LinearAllocator : public dtTileCacheAlloc
{
	unsigned char* buffer;
	size_t capacity;
	size_t top;
	size_t high; 
	LinearAllocator(const size_t cap);
	~LinearAllocator();
	void resize(const size_t cap);
	virtual void reset();
	virtual void* alloc(const size_t size);
	virtual void free(void* /*ptr*/);
}; 

class NavmeshBuilder {
public:
	RegionParam RegionData;//加载网格需要传入的参数
	InputGeom* Geom;//被加载的3D模型的结构体 
	BuildContext* Ctx;//日志结构
	PosV3 PolyPickExt;//周边的点位
	dtNavMesh* NavMesh;//构建的网格结构u

	struct LinearAllocator* Talloc;//线段分配
	struct FastLZCompressor* Tcomp;//压缩分配
	struct MeshProcess* Tmproc;//网格的分配 
	  
	dtTileCache* TileCache;//构建所需要的结构
public:
	NavmeshBuilder(BuildContext* ctx);
	~NavmeshBuilder(); 
	bool LoadGeometry(string paTileHeight);//加载一个模型
	void InitRegionData();
	bool BuildMap();
	int rasterizeTileLayers(const int tx, const int ty, const rcConfig& cfg, TileCacheData* tiles, const int maxTiles);
	dtNavMeshQuery* GeneratorNavMeshQuery();
};


inline MeshProcess::MeshProcess() : m_geom(0)
{
}
inline void MeshProcess::init(InputGeom* geom)
{
	m_geom = geom;
}

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
#define MAX_LAYERS  32 //���֧�ֶ��ٸ�
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
	RegionParam RegionData;//����������Ҫ����Ĳ���
	InputGeom* Geom;//�����ص�3Dģ�͵Ľṹ�� 
	BuildContext* Ctx;//��־�ṹ
	PosV3 PolyPickExt;//�ܱߵĵ�λ
	dtNavMesh* NavMesh;//����������ṹu

	struct LinearAllocator* Talloc;//�߶η���
	struct FastLZCompressor* Tcomp;//ѹ������
	struct MeshProcess* Tmproc;//����ķ��� 
	  
	dtTileCache* TileCache;//��������Ҫ�Ľṹ
public:
	NavmeshBuilder(BuildContext* ctx);
	~NavmeshBuilder(); 
	bool LoadGeometry(string paTileHeight);//����һ��ģ��
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

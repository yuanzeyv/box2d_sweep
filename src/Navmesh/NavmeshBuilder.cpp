#include "Recast/Recast.h"
#include "DetourTileCache/DetourTileCache.h"
#include "DetourTileCache/DetourTileCacheBuilder.h"
#include "Detour/DetourNavMeshQuery.h"
#include "Detour/DetourNavMeshBuilder.h"
#include "Navmesh/InputGeom.h"
#include "Detour/DetourCommon.h"
#include "Navmesh/SampleInterfaces.h"  
#include "DetourTileCache/DetourTileCache.h"
#include "Detour/DetourNavMesh.h"
#include "Navmesh/NavmeshBuilder.h"
#include "Navmesh/RegionParam.h"
#include <chrono>  
#include <map>
#include <stack>
#include "Box2d/box2d.h"
#include <stdint.h>  
#include "Library/fastlz/fastlz.h" 
using std::string;

int FastLZCompressor::maxCompressedSize(const int bufferSize)
{
	return (int)(bufferSize * 1.05f);
}

dtStatus FastLZCompressor::compress(const unsigned char* buffer, const int bufferSize, unsigned char* compressed, const int /*maxCompressedSize*/, int* compressedSize)
{
	*compressedSize = fastlz_compress((const void* const)buffer, bufferSize, compressed);
	return DT_SUCCESS;
}

dtStatus  FastLZCompressor::decompress(const unsigned char* compressed, const int compressedSize, unsigned char* buffer, const int maxBufferSize, int* bufferSize)
{
	*bufferSize = fastlz_decompress(compressed, compressedSize, buffer, maxBufferSize);
	return *bufferSize < 0 ? DT_FAILURE : DT_SUCCESS;
}


NavmeshBuilder::NavmeshBuilder(BuildContext* ctx) :PolyPickExt(2.f, 4.f, 2.f), Ctx(ctx),Geom(NULL), TileCache(NULL)
{
	Talloc = new LinearAllocator(32000);
	Tcomp = new FastLZCompressor();
	Tmproc = new MeshProcess();
}

NavmeshBuilder::~NavmeshBuilder()
{
	dtFreeTileCache(TileCache);//删除这个 
	delete Geom;
	delete Talloc;
	delete Tcomp;
	delete Tmproc;
	Talloc = NULL;
	Tcomp = NULL;
	Tmproc = NULL;
	Ctx = NULL;
	Geom = NULL;
	TileCache = NULL;
}

//加载一个模型
bool NavmeshBuilder::LoadGeometry(string paTileHeight) 
{
	if (Geom) return false;//已经加载过了 
	Geom = new InputGeom();//新建一个输入模型
	if (!Geom->load(Ctx, paTileHeight))
	{
		delete Geom;
		Geom = NULL;
		return false;
	}
	InitRegionData();
	return true; //加载模型失败的话  
} 

void NavmeshBuilder::InitRegionData()
{
	RegionData.BMin = Geom->getNavMeshBoundsMin();
	RegionData.BMax = Geom->getNavMeshBoundsMax();
	rcCalcGridSize(RegionData.BMin.Addr(), RegionData.BMax.Addr(), RegionData.CellSize, &RegionData.GridWidth, &RegionData.GridHeight);
	const int TileWidTileHeight = (RegionData.GridWidth + RegionData.TileSize - 1) / RegionData.TileSize;
	const int TileHeight = (RegionData.GridHeight + RegionData.TileSize - 1) / RegionData.TileSize;
	// Max tiles and max polys affect how TileHeighte tile IDs are caculated.
	// TileHeightere are 22 bits available for identifying a tile and a polygon.
	int tileBits = rcMin((int)dtIlog2(dtNextPow2(TileWidTileHeight * TileHeight * EXPECTED_LAYERS_PER_TILE)), 14);
	if (tileBits > 14) tileBits = 14;
	int polyBits = 22 - tileBits;
	RegionData.MaxTiles = 1 << tileBits;
	RegionData.MaxPolysPerTile = 1 << polyBits;
}
bool NavmeshBuilder::BuildMap()//构建地图
{
	dtStatus status;
	if (!Geom || !Geom->getMesh()) {//没有合适的地图的话
		Ctx->log(RC_LOG_ERROR, "buildTiledNavigation: No vertices and triangles.");
		return false;
	}
	Tmproc->init(Geom);
	// Init cache  
	const int TileWidth = (RegionData.GridWidth + RegionData.TileSize - 1) / RegionData.TileSize;
	const int TileHeight = (RegionData.GridHeight + RegionData.TileSize - 1) / RegionData.TileSize;
	// Generation params.
	rcConfig cfg;
	memset(&cfg, 0, sizeof(cfg));
	cfg.cs = RegionData.CellSize;
	cfg.ch = RegionData.CellHeight;
	cfg.walkableSlopeAngle = RegionData.AgentMaxSlope;
	cfg.walkableHeight = (int)ceilf(RegionData.AgenTileHeighteight / cfg.ch);
	cfg.walkableClimb = (int)floorf(RegionData.AgentMaxClimb / cfg.ch);
	cfg.walkableRadius = (int)ceilf(RegionData.AgentRadius / cfg.cs);
	cfg.maxEdgeLen = (int)(RegionData.EdgeMaxLen / RegionData.CellSize);
	cfg.maxSimplificationError = RegionData.EdgeMaxError;
	cfg.minRegionArea = (int)rcSqr(RegionData.RegionMinSize);		// Note: area = size*size
	cfg.mergeRegionArea = (int)rcSqr(RegionData.RegionMergeSize);	// Note: area = size*size
	cfg.maxVertsPerPoly = (int)RegionData.VertsPerPoly;
	cfg.tileSize = RegionData.TileSize;
	cfg.borderSize = cfg.walkableRadius + 3; // Reserve enough padding.
	cfg.width = cfg.tileSize + cfg.borderSize * 2;
	cfg.height = cfg.tileSize + cfg.borderSize * 2;
	cfg.detailSampleDist = RegionData.DetailSampleDist < 0.9f ? 0 : RegionData.CellSize * RegionData.DetailSampleDist;
	cfg.detailSampleMaxError = RegionData.CellHeight * RegionData.DetailSampleMaxError;
	rcVcopy(cfg.bmin, RegionData.BMin.Addr());
	rcVcopy(cfg.bmax, RegionData.BMax.Addr());
	// Tile cache params.
	dtTileCacheParams tcparams;
	memset(&tcparams, 0, sizeof(tcparams));
	rcVcopy(tcparams.orig, RegionData.BMin.Addr());
	tcparams.cs = RegionData.CellSize;
	tcparams.ch = RegionData.CellHeight;
	tcparams.width = RegionData.TileSize;
	tcparams.height = RegionData.TileSize;
	tcparams.walkableHeight = RegionData.AgenTileHeighteight;
	tcparams.walkableRadius = RegionData.AgentRadius;
	tcparams.walkableClimb = RegionData.AgentMaxClimb;
	tcparams.maxSimplificationError = RegionData.EdgeMaxError;
	tcparams.maxTiles = TileWidth * TileHeight * EXPECTED_LAYERS_PER_TILE;
	tcparams.maxObstacles = 128;
	dtFreeTileCache(TileCache);
	TileCache = dtAllocTileCache();
	if (!TileCache)
	{
		Ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate tile cache.");
		return false;
	}
	status = TileCache->init(&tcparams, Talloc, Tcomp, Tmproc);
	if (dtStatusFailed(status))
	{
		Ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init tile cache.");
		return false;
	}
	NavMesh = dtAllocNavMesh();
	if (!NavMesh)
	{
		Ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate navmesh.");
		return false;
	}
	dtNavMeshParams params;
	memset(&params, 0, sizeof(params));
	rcVcopy(params.orig, RegionData.BMin.Addr());
	params.tileWidth = RegionData.TileSize * RegionData.CellSize;
	params.tileHeight = RegionData.TileSize * RegionData.CellSize;
	params.maxTiles = RegionData.MaxTiles;
	params.maxPolys = RegionData.MaxPolysPerTile;
	status = NavMesh->init(&params);
	if (dtStatusFailed(status))
	{
		Ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init navmesh.");
		return false;
	}

	// Preprocess tiles.   
	for (int y = 0; y < TileHeight; ++y)
	{
		for (int x = 0; x < TileWidth; ++x)
		{
			TileCacheData tiles[MAX_LAYERS];
			memset(tiles, 0, sizeof(tiles));
			int ntiles = rasterizeTileLayers(x, y, cfg, tiles, MAX_LAYERS);
			for (int i = 0; i < ntiles; ++i) {
				TileCacheData* tile = &tiles[i];
				status = TileCache->addTile(tile->data, tile->dataSize, DT_COMPRESSEDTILE_FREE_DATA, 0);
				if (dtStatusFailed(status))
				{
					dtFree(tile->data);
					tile->data = 0;
					continue;
				}
			}
		}
	}
	for (int y = 0; y < TileHeight; ++y)
		for (int x = 0; x < TileWidth; ++x)
			TileCache->buildNavMeshTilesAt(x, y, NavMesh);
	const dtNavMesh* nav = NavMesh;
	int navmeshMemUsage = 0;
	for (int i = 0; i < nav->getMaxTiles(); ++i) {
		const dtMeshTile* tile = nav->getTile(i);
		if (tile->header)
			navmeshMemUsage += tile->dataSize;
	}
	printf("navmeshMemUsage = %.1f kB\n\r", navmeshMemUsage / 1024.0f);
	return  true;
}

dtNavMeshQuery* NavmeshBuilder::GeneratorNavMeshQuery()
{
	dtNavMeshQuery* meshQuery = dtAllocNavMeshQuery();
	dtStatus status = meshQuery->init(NavMesh, 2048);
	if (dtStatusFailed(status))
	{
		dtFreeNavMeshQuery(meshQuery);
		return NULL;
	}
	return meshQuery;
}
int NavmeshBuilder::rasterizeTileLayers(const int tx, const int ty, const rcConfig& cfg, TileCacheData* tiles, const int maxTiles)
{
	if (!Geom || !Geom->getMesh() || !Geom->getChunkyMesh())
	{
		Ctx->log(RC_LOG_ERROR, "buildTile: Input mesh is not specified.");
		return 0;
	}
	FastLZCompressor comp;
	RasterizationContext rc;

	const float* verts = Geom->getMesh()->getVerts();
	const int nverts = Geom->getMesh()->getVertCount();
	const rcChunkyTriMesh* chunkyMesh = Geom->getChunkyMesh();

	// Tile bounds.
	const float tcs = cfg.tileSize * cfg.cs;

	rcConfig tcfg;
	memcpy(&tcfg, &cfg, sizeof(tcfg));

	tcfg.bmin[0] = cfg.bmin[0] + tx * tcs;
	tcfg.bmin[1] = cfg.bmin[1];
	tcfg.bmin[2] = cfg.bmin[2] + ty * tcs;
	tcfg.bmax[0] = cfg.bmin[0] + (tx + 1) * tcs;
	tcfg.bmax[1] = cfg.bmax[1];
	tcfg.bmax[2] = cfg.bmin[2] + (ty + 1) * tcs;
	tcfg.bmin[0] -= tcfg.borderSize * tcfg.cs;
	tcfg.bmin[2] -= tcfg.borderSize * tcfg.cs;
	tcfg.bmax[0] += tcfg.borderSize * tcfg.cs;
	tcfg.bmax[2] += tcfg.borderSize * tcfg.cs;

	// Allocate voxel heightfield where we rasterize our input data to.
	rc.solid = rcAllocHeightfield();
	if (!rc.solid)
	{
		Ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
		return 0;
	}
	if (!rcCreateHeightfield(Ctx, *rc.solid, tcfg.width, tcfg.height, tcfg.bmin, tcfg.bmax, tcfg.cs, tcfg.ch))
	{
		Ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
		return 0;
	}

	// Allocate array TileHeightat can hold triangle flags.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold TileHeighte max number of triangles you need to process.
	rc.triareas = new unsigned char[chunkyMesh->maxTrisPerChunk];
	if (!rc.triareas)
	{
		Ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", chunkyMesh->maxTrisPerChunk);
		return 0;
	}

	float tbmin[2], tbmax[2];
	tbmin[0] = tcfg.bmin[0];
	tbmin[1] = tcfg.bmin[2];
	tbmax[0] = tcfg.bmax[0];
	tbmax[1] = tcfg.bmax[2];
	int cid[512];// TODO: Make grow when returning too many items.
	const int ncid = rcGetChunksOverlappingRect(chunkyMesh, tbmin, tbmax, cid, 512);
	if (!ncid)
	{
		return 0; // empty
	}

	for (int i = 0; i < ncid; ++i)
	{
		const rcChunkyTriMeshNode& node = chunkyMesh->nodes[cid[i]];
		const int* tris = &chunkyMesh->tris[node.i * 3];
		const int ntris = node.n;

		memset(rc.triareas, 0, ntris * sizeof(unsigned char));
		rcMarkWalkableTriangles(Ctx, tcfg.walkableSlopeAngle, verts, nverts, tris, ntris, rc.triareas);

		if (!rcRasterizeTriangles(Ctx, verts, nverts, tris, rc.triareas, ntris, *rc.solid, tcfg.walkableClimb))
			return 0;
	}
	// Once all geometry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by TileHeighte conservative rasterization
	// as well as filter spans where TileHeighte character cannot possibly stand.
	if (RegionData.FilterLowHangingObstacles)
		rcFilterLowHangingWalkableObstacles(Ctx, tcfg.walkableClimb, *rc.solid);
	if (RegionData.FilterLedgeSpans)
		rcFilterLedgeSpans(Ctx, tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid);
	if (RegionData.FilterWalkableLowHeightSpans)
		rcFilterWalkableLowHeightSpans(Ctx, tcfg.walkableHeight, *rc.solid);

	rc.chf = rcAllocCompactHeightfield();
	if (!rc.chf)
	{
		Ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
		return 0;
	}
	if (!rcBuildCompactHeightfield(Ctx, tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid, *rc.chf))
	{
		Ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
		return 0;
	}

	// Erode TileHeighte walkable area by agent radius.
	if (!rcErodeWalkableArea(Ctx, tcfg.walkableRadius, *rc.chf))
	{
		Ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
		return 0;
	}

	// (Optional) Mark areas.
	const ConvexVolume* vols = Geom->getConvexVolumes();
	for (int i = 0; i < Geom->getConvexVolumeCount(); ++i)
	{
		rcMarkConvexPolyArea(Ctx, vols[i].verts, vols[i].nverts,
			vols[i].hmin, vols[i].hmax,
			(unsigned char)vols[i].area, *rc.chf);
	}

	rc.lset = rcAllocHeightfieldLayerSet();
	if (!rc.lset)
	{
		Ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'lset'.");
		return 0;
	}
	if (!rcBuildHeightfieldLayers(Ctx, *rc.chf, tcfg.borderSize, tcfg.walkableHeight, *rc.lset))
	{
		Ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build heighfield layers.");
		return 0;
	}

	rc.ntiles = 0;
	for (int i = 0; i < rcMin(rc.lset->nlayers, MAX_LAYERS); ++i)
	{
		TileCacheData* tile = &rc.tiles[rc.ntiles++];
		const rcHeightfieldLayer* layer = &rc.lset->layers[i];

		// Store header
		dtTileCacheLayerHeader header;
		header.magic = DT_TILECACHE_MAGIC;
		header.version = DT_TILECACHE_VERSION;

		// Tile layer location in TileHeighte navmesh.
		header.tx = tx;
		header.ty = ty;
		header.tlayer = i;
		dtVcopy(header.bmin, layer->bmin);
		dtVcopy(header.bmax, layer->bmax);

		// Tile info.
		header.width = (unsigned char)layer->width;
		header.height = (unsigned char)layer->height;
		header.minx = (unsigned char)layer->minx;
		header.maxx = (unsigned char)layer->maxx;
		header.miny = (unsigned char)layer->miny;
		header.maxy = (unsigned char)layer->maxy;
		header.hmin = (unsigned short)layer->hmin;
		header.hmax = (unsigned short)layer->hmax;

		dtStatus status = dtBuildTileCacheLayer(&comp, &header, layer->heights, layer->areas, layer->cons,
			&tile->data, &tile->dataSize);
		if (dtStatusFailed(status))
		{
			return 0;
		}
	}

	// Transfer ownsership of tile data from build context to TileHeighte caller.
	int n = 0;
	for (int i = 0; i < rcMin(rc.ntiles, maxTiles); ++i)
	{
		tiles[n++] = rc.tiles[i];
		rc.tiles[i].data = 0;
		rc.tiles[i].dataSize = 0;
	}
	return n;
}
RasterizationContext::RasterizationContext() :solid(0), triareas(0), lset(0), chf(0), ntiles(0)
{
	memset(tiles, 0, sizeof(TileCacheData) * MAX_LAYERS);
}

RasterizationContext::~RasterizationContext()
{
	rcFreeHeightField(solid);
	delete[] triareas;
	rcFreeHeightfieldLayerSet(lset);
	rcFreeCompactHeightfield(chf);
	for (int i = 0; i < MAX_LAYERS; ++i)
	{
		dtFree(tiles[i].data);
		tiles[i].data = 0;
	}
}

void MeshProcess::process(struct dtNavMeshCreateParams* params, unsigned char* polyAreas, unsigned short* polyFlags)
{
	// Update poly flags from areas.
	for (int i = 0; i < params->polyCount; ++i)
	{
		if (polyAreas[i] == DT_TILECACHE_WALKABLE_AREA)
			polyAreas[i] = SAMPLE_POLYAREA_GROUND;
		if (polyAreas[i] == SAMPLE_POLYAREA_GROUND || polyAreas[i] == SAMPLE_POLYAREA_GRASS || polyAreas[i] == SAMPLE_POLYAREA_ROAD)
		{
			polyFlags[i] = SAMPLE_POLYFLAGS_WALK;
		}
		else if (polyAreas[i] == SAMPLE_POLYAREA_WATER)
		{
			polyFlags[i] = SAMPLE_POLYFLAGS_SWIM;
		}
		else if (polyAreas[i] == SAMPLE_POLYAREA_DOOR)
		{
			polyFlags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
		}
	}
	// Pass in off-mesh connections.
	if (m_geom)
	{
		params->offMeshConVerts = m_geom->getOffMeshConnectionVerts();
		params->offMeshConRad = m_geom->getOffMeshConnectionRads();
		params->offMeshConDir = m_geom->getOffMeshConnectionDirs();
		params->offMeshConAreas = m_geom->getOffMeshConnectionAreas();
		params->offMeshConFlags = m_geom->getOffMeshConnectionFlags();
		params->offMeshConUserID = m_geom->getOffMeshConnectionId();
		params->offMeshConCount = m_geom->getOffMeshConnectionCount();
	}
}
LinearAllocator::LinearAllocator(const size_t cap) : buffer(0), capacity(0), top(0), high(0)
{
	resize(cap);
}

LinearAllocator::~LinearAllocator()
{
	dtFree(buffer);
}

void LinearAllocator::resize(const size_t cap)
{
	if (buffer) dtFree(buffer);
	buffer = (unsigned char*)dtAlloc(cap, DT_ALLOC_PERM);
	capacity = cap;
}

void LinearAllocator::reset()
{
	high = dtMax(high, top);
	top = 0;
}

void* LinearAllocator::alloc(const size_t size)
{
	if (!buffer)
		return 0;
	if (top + size > capacity)
		return 0;
	unsigned char* mem = &buffer[top];
	top += size;
	return mem;
}

void LinearAllocator::free(void* /*ptr*/)
{
	// Empty
}

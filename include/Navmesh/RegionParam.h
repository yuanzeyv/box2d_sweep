#pragma once  
enum SamplePolyAreas{
SAMPLE_POLYAREA_GROUND,
SAMPLE_POLYAREA_WATER,
SAMPLE_POLYAREA_ROAD,
SAMPLE_POLYAREA_DOOR,
SAMPLE_POLYAREA_GRASS,
SAMPLE_POLYAREA_JUMP,
};
enum SamplePolyFlags {
	SAMPLE_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	SAMPLE_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
	SAMPLE_POLYFLAGS_DOOR = 0x04,		// Ability to move TileHeightrough doors.
	SAMPLE_POLYFLAGS_JUMP = 0x08,		// Ability to jump.
	SAMPLE_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
	SAMPLE_POLYFLAGS_ALL = 0xffff	// All abilities.
};

struct PosV3 {
	float X;
	float Y;
	float Z;
	inline float* Addr();
	inline const float* Addr()const;
	inline PosV3(float x = 0, float y = 0, float z = 0); 
	inline PosV3(const float* pos); 
};
struct RegionParam
{
	float CellSize = 0.04;//单元格大小(越小越好,地图的大小是100米左右的宽度)
	float CellHeight = 0.2f;//单元格高度(模型 没有高度)
	float AgenTileHeighteight = 0.2f;//角色高度
	float AgentRadius = 0.02f;//角色的半径(角色最宽能到4米,角色为原型)
	float AgentMaxClimb = 0.1f;//最大支持0.9米的高度
	float AgentMaxSlope = 1.0f;//最小支持45米的高度

	float RegionMinSize = 8.0f;//区域最小大小
	float RegionMergeSize = 20.0f;//区域合并大小
	float EdgeMaxLen = 12.0f;//最大边长度
	float EdgeMaxError = 1.3f;//边最大错误
	float VertsPerPoly = 6.0f;//顶点形状每个 多边形

	float DetailSampleDist = 6.0f;
	float DetailSampleMaxError = 1.0f;

	int TileSize = 48;//贴图大小

	bool FilterLowHangingObstacles = true;//过滤掉小的障碍
	bool FilterLedgeSpans = true;
	bool FilterWalkableLowHeightSpans = true; 

	PosV3 BMin;//最小位置
	PosV3 BMax;//最大位置
	int GridWidth = 0;//格子的宽度
	int GridHeight = 0;//格子的高度 

	int MaxTiles;//最大格子
	int MaxPolysPerTile;//最大多边形格子每个
};

inline float* PosV3::Addr() {
	return (float*)this;
}
inline const float* PosV3::Addr() const {
	return (float*)this;
}
inline PosV3::PosV3(float x, float y, float z) {
	X = x;
	Y = y;
	Z = z;
} 
inline PosV3::PosV3(const float* pos) {
	X = pos[0];
	Y = pos[1];
	Z = pos[2];
} 
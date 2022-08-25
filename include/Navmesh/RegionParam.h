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
	float CellSize = 0.04;//��Ԫ���С(ԽСԽ��,��ͼ�Ĵ�С��100�����ҵĿ��)
	float CellHeight = 0.2f;//��Ԫ��߶�(ģ�� û�и߶�)
	float AgenTileHeighteight = 0.2f;//��ɫ�߶�
	float AgentRadius = 0.02f;//��ɫ�İ뾶(��ɫ����ܵ�4��,��ɫΪԭ��)
	float AgentMaxClimb = 0.1f;//���֧��0.9�׵ĸ߶�
	float AgentMaxSlope = 1.0f;//��С֧��45�׵ĸ߶�

	float RegionMinSize = 8.0f;//������С��С
	float RegionMergeSize = 20.0f;//����ϲ���С
	float EdgeMaxLen = 12.0f;//���߳���
	float EdgeMaxError = 1.3f;//��������
	float VertsPerPoly = 6.0f;//������״ÿ�� �����

	float DetailSampleDist = 6.0f;
	float DetailSampleMaxError = 1.0f;

	int TileSize = 48;//��ͼ��С

	bool FilterLowHangingObstacles = true;//���˵�С���ϰ�
	bool FilterLedgeSpans = true;
	bool FilterWalkableLowHeightSpans = true; 

	PosV3 BMin;//��Сλ��
	PosV3 BMax;//���λ��
	int GridWidth = 0;//���ӵĿ��
	int GridHeight = 0;//���ӵĸ߶� 

	int MaxTiles;//������
	int MaxPolysPerTile;//������θ���ÿ��
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
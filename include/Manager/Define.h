#pragma once
//地图比例尺
#define PlottingScale (0.1f)						 //比例尺, 0.1米 = 1米
#define DistanceTrans(meter) (meter * PlottingScale) //实际距离转换为 比例尺

enum ViewType {
	VIEW_TYPE_NULL = 0,	//这是一个空单元
	MONSTER_HERO = 1,//怪物表
	HERO_MONSTER = 2,//英雄表 
	HERO_HERO = 3,//英雄到英雄
	STATIC_HERO = 4,//静态到英雄
	HERO_STATIC = 5,//英雄到静态
	HERO_BULLET = 6,//英雄到子弹
	BULLET_HERO = 7,//子弹到英雄
};
enum ViewStatus {
	VISIBLE = 0,
	INVISIBLE = 1,
};

//4种物品属性
enum BodyType {
	BODY_TYPE_NULL = 0,
	STATIC_BODY = 1,
	PLAYER_BODY = 2,
	MONSTER_BODY = 3,
	BULLET_BODY = 4,
};
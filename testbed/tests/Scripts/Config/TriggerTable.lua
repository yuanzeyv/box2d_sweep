local table = {
[1] = {
	id = 1,
	TriggerName = "碰撞时销毁一个单元",
	DESC = "碰撞调用",
	Type = 1,
	Param1 = "对象(1:自己 2对方 3:视野最近的目标 4:视野最远的目标)",
	Param2 = "是否时延",
},
[2] = {
	id = 2,
	TriggerName = "碰撞时添加碰撞弹性",
	DESC = "触发时,添加一个弹力",
	Type = 1,
	Param1 = "弹力 0-1",
},
[3] = {
	id = 3,
	TriggerName = "碰撞时添加一个摩擦",
	DESC = "碰撞后，对碰撞点添加一个摩擦",
	Type = 1,
	Param1 = "摩擦力（0-1）",
},
[4] = {
	id = 4,
	TriggerName = "碰撞时添加一个爆炸",
	DESC = "碰撞后，对指定点添加一个瞬发的爆炸 ",
	Type = 1,
	Param1 = "对象(1:自己 2对方 3:视野最近的目标 4:视野最远的目标)",
	Param2 = "是否时延",
	Param3 = "爆炸半径",
},
[5] = {
	id = 5,
	TriggerName = "切线移动触发（建筑用）",
	DESC = "触发时，对对象添加一个切线移动",
	Type = 1,
	Param1 = "速度 0-1",
},
[6] = {
	id = 6,
	TriggerName = "碰撞时,根据碰撞类型 确定是否拦截对象",
	DESC = "将拦截除释放者外的所有非墙体",
	Type = 1,
	Param1 = "时长（s）",
	Param2 = "长度（米）",
	Param3 = "宽度（米）",
},
[7] = {
	id = 7,
	TriggerName = "碰撞后，传送到指定的建筑下",
	DESC = "触发后，将玩家传送到指定 建筑ID旁",
	Type = 0,
	Param1 = "地图建筑ID",
	Param2 = "是否时延",
},
[8] = {
	id = 8,
	TriggerName = "销毁触发",
	DESC = "触发时销毁目标对象",
	Type = 0,
	Param1 = "时延销毁",
	Param3 = " ",
	Param4 = " ",
},
[9] = {
	id = 9,
	TriggerName = "爆炸触发",
	DESC = "触发时，赋予一个带有半径的爆炸",
	Type = 0,
	Param1 = "半径（米）",
},
[10] = {
	id = 10,
	TriggerName = "传送建筑",
	DESC = "触发后，将玩家传送到指定 建筑ID旁",
	Type = 0,
	Param1 = "地图建筑ID",
},
[11] = {
	id = 11,
	TriggerName = "创建怪物建筑",
	DESC = "触发后，在指定坐标创建一个怪物",
	Type = 0,
	Param1 = "建筑ID",
	Param2 = "怪物ID",
},
[12] = {
	id = 12,
	TriggerName = "循环触发",
	DESC = "循环多少次，执行某一个触发",
	Type = 0,
	Param1 = "循环次数（-1代表永循环）",
	Param2 = "Delay(时延)",
	Param3 = "是否立即执行一次",
	Param4 = "执行字符串",
},
[13] = {
	id = 13,
	TriggerName = "碰撞时，对碰撞对象造成伤害",
	DESC = "造成角色等级对应的伤害",
	Type = 1,
},

}
return table

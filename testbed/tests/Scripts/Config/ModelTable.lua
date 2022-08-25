local table = {

[1] = {

	id = 1,

	ShapePath = "MiniMap",

	architectureName = "静态地图建筑",

--	BeginCollider = "5|1#13",

	ISSensor = 0,

	IsStatic = 1,

},

[2] = {

	id = 2,

	ShapePath = "Player",

	architectureName = "玩家角色",

	BeginCollider = "12|1",

	ISSensor = 0,

	IsStatic = 0,

},

[3] = {

	id = 3,

	ShapePath = "MonsterGen",

	architectureName = "怪物生成",

	ISSensor = 0,

	IsStatic = 1,

},

[4] = {

	id = 4,

	ShapePath = "Player",

	architectureName = "怪物角色",

	ISSensor = 0,

	IsStatic = 0, 
},



}

return table


EnumDefine = {} 
EnumDefine.BodyType = {} 
EnumDefine.BodyType.NONE = 0
EnumDefine.BodyType.StaticBody = 1
EnumDefine.BodyType.PlayerBody = 2
EnumDefine.BodyType.MonsterBody = 3
EnumDefine.BodyType.BulletBody = 4 


EnumDefine.b2BodyType = {}  
EnumDefine.b2BodyType.b2_staticBody = 0
EnumDefine.b2BodyType.b2_kinematicBody = 1 
EnumDefine.b2BodyType.b2_dynamicBody = 2
 
EnumDefine.b2ShapeType = {} 
EnumDefine.b2ShapeType.Circle = 0
EnumDefine.b2ShapeType.Edge = 1
EnumDefine.b2ShapeType.Polygon = 2
EnumDefine.b2ShapeType.Chain = 3
 
EnumDefine.ColliderType = {}
EnumDefine.ColliderType.BeginContact = 0
EnumDefine.ColliderType.EndContact = 1
EnumDefine.ColliderType.PreSolve = 2
EnumDefine.ColliderType.PostSolve = 3 

EnumDefine.ViewType = {} 
EnumDefine.ViewType.Null = 0	--这是一个空单元
EnumDefine.ViewType.Monster_Hero = 1 --怪物表
EnumDefine.ViewType.Hero_Monster = 2 --英雄表 
EnumDefine.ViewType.Hero_Hero = 3--英雄到英雄
EnumDefine.ViewType.Static_Hero = 4--静态到英雄
EnumDefine.ViewType.Hero_Static = 5--英雄到静态
EnumDefine.ViewType.Hero_Bullet = 6--英雄到子弹
EnumDefine.ViewType.Bullet_Hero = 7--子弹到英雄 


EnumDefine.ViewStatus = {} 
EnumDefine.ViewStatus.Visble = 0
EnumDefine.ViewStatus.Invisble = 1

--属性系统有哪几个
EnumDefine.AttributeSystem = {} 
EnumDefine.AttributeSystem.LevelSystem = 0
EnumDefine.AttributeSystem.EquipSystem = 1


--属性类型
--(1-1000 是基础属性 )
--(1000 - 2000是附加属性)
--(10000 - 11000 是基础属性加成)
--(11000 - 12000 是附加属性加成)
--(20000 - 21000 是总属性加成)
EnumDefine.AttributeKey = {} 
EnumDefine.AttributeKey.HP = 0
EnumDefine.AttributeKey.MP = 1
EnumDefine.AttributeKey.Attack = 2
EnumDefine.AttributeKey.MagicAttack = 3
EnumDefine.AttributeKey.CirtPercent = 4
EnumDefine.AttributeKey.CirtHarmPercent = 5
EnumDefine.AttributeKey.CDReduce = 6
EnumDefine.AttributeKey.UpLevelExp = 7
EnumDefine.AttributeKey.KillAwardExp = 8
EnumDefine.AttributeKey.DieDropExp = 9 


EnumDefine.AttributeType = {} 
EnumDefine.AttributeType.Base = 0
EnumDefine.AttributeType.Addition = 1000
EnumDefine.AttributeType.BasePercent = 2000
EnumDefine.AttributeType.AdditionPercent = 3000
EnumDefine.AttributeType.SumPercent = 4000
 
--行为树的 状态
EnumDefine.NodeStatus = {} 
EnumDefine.NodeStatus.IDLE = 0
EnumDefine.NodeStatus.RUNNING = 1 
EnumDefine.NodeStatus.SUCCESS = 2 
EnumDefine.NodeStatus.FAILURE = 3  

--针对字节的直接操作
EnumDefine.BitsStatus = {} 
EnumDefine.BitsStatus.Enable = 1 
EnumDefine.BitsStatus.Disable = 0


--玩家移动行为树中的移动状态
EnumDefine.MoveBehavior = {} 
EnumDefine.MoveBehavior.Reday = 0  --移动刚开始
EnumDefine.MoveBehavior.Running = 1 --移动中
EnumDefine.MoveBehavior.Over = 2  --移动完成了


--玩家针对行为树的一些动作
EnumDefine.PlayerActionBits = {} 
EnumDefine.PlayerActionBits.ALL = 0xFFFFFFFF 
EnumDefine.PlayerActionBits.MOVE = 1 >> 0 

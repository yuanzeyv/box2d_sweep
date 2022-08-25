local BaseProxy = require("Logic.Proxy.BaseProxy")
local ActorBehaivorProxy = class("ActorBehaivorProxy",BaseProxy) 
local PlayerBehavior = require("ActorBehaivorTree.PlayerBehavior")--行为树模板
local MonsterBehavior = require("ActorBehaivorTree.MonsterBehavior")--行为树模板
function ActorBehaivorProxy:InitData()  
	self.BehaivorTreeFactoryTable = {} --用于存储当前的行为树

	self.PlayerMap = {} 
	self.Count = 0
end 

function ActorBehaivorProxy:LoadFinish() 
	self:InitBehaivorTreeFactory() --初始化工厂
end  
function ActorBehaivorProxy:InitBehaivorTreeFactory()
	local factoryKey = {
		[EnumDefine.BodyType.PlayerBody]= PlayerBehavior.new(),
		[EnumDefine.BodyType.MonsterBody]= MonsterBehavior.new(),
	}
	for v,k in pairs(factoryKey) do 
		self.BehaivorTreeFactoryTable[v] = k.new()
	end 
end 
 
function ActorBehaivorProxy:CreateBehavior(treeType,actor) 
	if not self.BehaivorTreeFactoryTable[treeType] then 
		return 
	end 
	return self.BehaivorTreeFactoryTable[treeType]:CreateTree({Actor = actor});  
end  
function ActorBehaivorProxy:RegisterPlayer(actor)--注册一个玩家 
	local behaviorTree = self:CreateBehavior(actor:GetActorType(),actor)
	if not behaviorTree then 
		return false
	end 
	self.PlayerMap[actor:GetID()] =  behaviorTree --传入这个玩家的ID 
	self.Count = self.Count + 1
	return true
end  
function ActorBehaivorProxy:ClearData() 
end  
function ActorBehaivorProxy:Step(time)
	for v,actorBehavior in pairs(self.PlayerMap) do  
		actorBehavior:tickRoot()--开始运行当前设备树  ()
	end  
end  
return ActorBehaivorProxy

  
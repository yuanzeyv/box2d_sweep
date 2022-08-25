local BaseProxy = require("Logic.Proxy.BaseProxy")
local ActorBehaivorProxy = class("ActorBehaivorProxy",BaseProxy) 
local PlayerBehavior = require("ActorBehaivorTree.PlayerBehavior")--��Ϊ��ģ��
local MonsterBehavior = require("ActorBehaivorTree.MonsterBehavior")--��Ϊ��ģ��
function ActorBehaivorProxy:InitData()  
	self.BehaivorTreeFactoryTable = {} --���ڴ洢��ǰ����Ϊ��

	self.PlayerMap = {} 
	self.Count = 0
end 

function ActorBehaivorProxy:LoadFinish() 
	self:InitBehaivorTreeFactory() --��ʼ������
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
function ActorBehaivorProxy:RegisterPlayer(actor)--ע��һ����� 
	local behaviorTree = self:CreateBehavior(actor:GetActorType(),actor)
	if not behaviorTree then 
		return false
	end 
	self.PlayerMap[actor:GetID()] =  behaviorTree --���������ҵ�ID 
	self.Count = self.Count + 1
	return true
end  
function ActorBehaivorProxy:ClearData() 
end  
function ActorBehaivorProxy:Step(time)
	for v,actorBehavior in pairs(self.PlayerMap) do  
		actorBehavior:tickRoot()--��ʼ���е�ǰ�豸��  ()
	end  
end  
return ActorBehaivorProxy

  
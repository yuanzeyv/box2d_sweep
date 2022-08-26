local BaseProxy = require("Logic.Proxy.BaseProxy")
local ActorFacadeProxy = class("ActorFacadeProxy",BaseProxy)   
function ActorFacadeProxy:InitData()    
end 

function ActorFacadeProxy:LoadFinish()   
    self.ActorProxy = self:getFacade():retrieveProxy(ProxyTable.ActorProxy)
    self.ActorAttributeManagerProxy = self:getFacade():retrieveProxy(ProxyTable.ActorAttributeProxy)
    self.MapProxy = self:getFacade():retrieveProxy(ProxyTable.MapProxy)   
    self.ActorBehaivorProxy = self:getFacade():retrieveProxy(ProxyTable.ActorBehaivorProxy) 
end   

--通过地图创建一个角色
function ActorFacadeProxy:CreateActor_GameMap(gameMapID)
    local mapCellConfig = assert(self.MapProxy:GetMapTable(gameMapID),"要创建的单元不存在 ") 
    return self:CreateActor(mapCellConfig.MonsterModel,mapCellConfig.ActorID,SplitB2Vec2(mapCellConfig.POS),mapCellConfig.MonsterLevel) 
end
--直接创建一个角色
function ActorFacadeProxy:CreateActor(modelID,actorID,pos,level)
    modelID = assert(tonumber(modelID),"模型ID为空")
    actorID = assert(tonumber(actorID),"角色ID为空") 
    assert(pos,"位置为空")
    level =  level or 1 
    local ActorBase = self.ActorProxy:CreateActor(actorID,modelID,pos,level)--创建一个角色，暂时不注册  
    local ID = ActorBase:GetID()
    self.ActorAttributeManagerProxy:RegisterActor(ID)--将角色给到属性中去 
    self.ActorAttributeManagerProxy:UpdateSystemTable(ID,EnumDefine.AttributeSystem.LevelSystem,ActorBase:GetLevelConfig())--将角色给到属性中去   
    self.ActorBehaivorProxy:RegisterPlayer(ActorBase) 
    ActorBase:InitCoreAttr() 
    ActorBase:OpenTimer()    
    return ActorBase  
end
--删除一个角色 
function ActorFacadeProxy:DestoryActor(ID)
    assert(ID,"传入的角色ID为空")  
    ID = tonumber(ID)
    self.ActorProxy:RemoveActor(ID)--删除角色信息
    self.ActorAttributeManagerProxy:UnRegisterActor(ID)--删除刚体信息 
end 

--获取到一个角色
function ActorFacadeProxy:GetActor(actorID)
    return self.ActorProxy:GetActor(actorID)
end  

--获取到一个角色的属性
function ActorFacadeProxy:GetAttrValue(actorID,attrType)
    return self.ActorAttributeManagerProxy:GetAttrValue(actorID,attrType) 
end  

--获取到一个角色的属性
function ActorFacadeProxy:RecalcActorCoreAttr(actorID)
    local actor = self.ActorProxy:GetActor(actorID) 
    if not actor then return end 
    actor:ReCalcCoreAttr()
end 
 

--初始化角色的核心属性
function ActorFacadeProxy:ClearData(actorID)
    local keyTable = self.ActorProxy:GetActorListKey() 
    for v,k in pairs(keyTable) do 
        self:DestoryActor(k) 
    end  
end 
return ActorFacadeProxy
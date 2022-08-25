local BaseProxy = require("Logic.Proxy.BaseProxy")
local ActorProxy = class("ActorProxy",BaseProxy)   
local ActorBase = require("ActorBase/ActorBase")
local TimerBase = require("ActorBase/TimerBase")
function ActorProxy:InitData() 
    self.ActorConfigTable = require "Config.ActorMapTable"--角色表  
    self.ModelConfigTable =  require "Config.ModelTable"--建筑表  
    self.AttributeConfigTable = {} --属性表
    self:InitAttributeConfig() --初始化属性表
    --游戏角色表
    self.ActorTable = {} 
end  
function ActorProxy:LoadFinish() 
    self.TriggerProxy = self:getFacade():retrieveProxy(ProxyTable.TriggerProxy) 
end   
function ActorProxy:InitAttributeConfig()
    for v,k in pairs(self.ActorConfigTable) do 
        if k.archLevelTable and not self.AttributeConfigTable[k.archLevelTable] then  
            self.AttributeConfigTable[k.archLevelTable] = require("Config/MonsterAttribute/" .. k.archLevelTable) --读属性表 
        end 
    end 
end    
function ActorProxy:GetModelConfigCell(modelID) 
    return self.ModelConfigTable[modelID]
end
function ActorProxy:GetActorConfigCell(actorKey) 
    return self.ActorConfigTable[actorKey]
end
function ActorProxy:ActorKeyIsExist(actorKey)
    return self.ActorConfigTable[actorKey] ~= nil
end  

function ActorProxy:AnysisTimer(actor,actorTable)
    local timerList = {} 
    for i=1,3,1 do 
        if actorTable["Time" .. i .. "Param"] then 
            local param = SplitParam(actorTable["Time" .. i .. "Param"])
            local executeList = SplitAttribute(actorTable["Time" .. i .. "Execute"]) 
            local executeTimer = TimerBase.new(actor,param.param1,param.param2,executeList) 
            timerList[i] = executeTimer
        end
    end
    return timerList
end 
function ActorProxy:CreateBody(archID,pos,actorType) --开始创建一个建筑 
    local modelConfig = self:GetModelConfigCell(archID)--获取到建筑表信息
    local BodyTable = {} --设置详细信息
    BodyTable.Body = sol.BodyMan.Instance():CreateBody(actorType, modelConfig.ShapePath,pos)--创建一个刚人体   
    BodyTable.ID = BodyTable.Body:ID()  
    BodyTable.ModelTriggerTable = {}  
    BodyTable.ModelTriggerTable.BeginCollider = SplitAttribute(modelConfig.BeginCollider)   
    BodyTable.ModelTriggerTable.EndCollider = SplitAttribute(modelConfig.EndCollider)   
    BodyTable.ModelTriggerTable.ContinueCollider = SplitAttribute(modelConfig.ContinueCollider)       
    local BodyType = (modelConfig.IsStatic == 1 and EnumDefine.b2BodyType.b2_staticBody) or EnumDefine.b2BodyType.b2_dynamicBody    
    local Sensor =  modelConfig.ISSensor
    BodyTable.Body:SetType(BodyType) --设置刚体的刚体类型  
    --BodyTable.Body:SetPosition(pos)   
    return BodyTable
end   

function ActorProxy:CreateActor(actorKey,archID,pos,level) 
    if not self:ActorKeyIsExist(actorKey) then 
        return nil
    end  
    --创建一个 角色
    local actorBase = ActorBase.new() 
    local actorConfig = self:GetActorConfigCell(actorKey) 
    actorBase:SetLevelTable(self.AttributeConfigTable[actorConfig.archLevelTable])--读到等级表
    actorBase:SetLevel(level)--设置角色等级 
    actorBase:SetTimer(self:AnysisTimer(actorBase,actorConfig) )--每个角色有三个定时器，每个定时器都有一个开关  
    actorBase:SetActorType(actorConfig.ActorType) --角色类型
    actorBase:SetBody(self:CreateBody(archID,pos,actorConfig.ActorType))
    self.ActorTable[actorBase:GetID()] = actorBase   
    
    return actorBase
end 
function ActorProxy:RemoveActor(actorID)  
    self.ActorTable[actorID]:Destory()  --删除自己的一些信息
    self.ActorTable[actorID] = nil
end
function ActorProxy:GetActor(actorID)
    return self.ActorTable[actorID]
end
function ActorProxy:GetActorListKey()
    return GetTableKey(self.ActorTable)
end 
return ActorProxy
local BaseProxy = require("Logic.Proxy.BaseProxy")
local ActorProxy = class("ActorProxy",BaseProxy)   
local ActorBase = require("ActorBase/ActorBase")
local TimerBase = require("ActorBase/TimerBase")
function ActorProxy:InitData() 
    self.ActorConfigTable = require "Config.ActorMapTable"--��ɫ��  
    self.ModelConfigTable =  require "Config.ModelTable"--������  
    self.AttributeConfigTable = {} --���Ա�
    self:InitAttributeConfig() --��ʼ�����Ա�
    --��Ϸ��ɫ��
    self.ActorTable = {} 
end  
function ActorProxy:LoadFinish() 
    self.TriggerProxy = self:getFacade():retrieveProxy(ProxyTable.TriggerProxy) 
end   
function ActorProxy:InitAttributeConfig()
    for v,k in pairs(self.ActorConfigTable) do 
        if k.archLevelTable and not self.AttributeConfigTable[k.archLevelTable] then  
            self.AttributeConfigTable[k.archLevelTable] = require("Config/MonsterAttribute/" .. k.archLevelTable) --�����Ա� 
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
function ActorProxy:CreateBody(archID,pos,actorType) --��ʼ����һ������ 
    local modelConfig = self:GetModelConfigCell(archID)--��ȡ����������Ϣ
    local BodyTable = {} --������ϸ��Ϣ
    BodyTable.Body = sol.BodyMan.Instance():CreateBody(actorType, modelConfig.ShapePath,pos)--����һ��������   
    BodyTable.ID = BodyTable.Body:ID()  
    BodyTable.ModelTriggerTable = {}  
    BodyTable.ModelTriggerTable.BeginCollider = SplitAttribute(modelConfig.BeginCollider)   
    BodyTable.ModelTriggerTable.EndCollider = SplitAttribute(modelConfig.EndCollider)   
    BodyTable.ModelTriggerTable.ContinueCollider = SplitAttribute(modelConfig.ContinueCollider)       
    local BodyType = (modelConfig.IsStatic == 1 and EnumDefine.b2BodyType.b2_staticBody) or EnumDefine.b2BodyType.b2_dynamicBody    
    local Sensor =  modelConfig.ISSensor
    BodyTable.Body:SetType(BodyType) --���ø���ĸ�������  
    --BodyTable.Body:SetPosition(pos)   
    return BodyTable
end   

function ActorProxy:CreateActor(actorKey,archID,pos,level) 
    if not self:ActorKeyIsExist(actorKey) then 
        return nil
    end  
    --����һ�� ��ɫ
    local actorBase = ActorBase.new() 
    local actorConfig = self:GetActorConfigCell(actorKey) 
    actorBase:SetLevelTable(self.AttributeConfigTable[actorConfig.archLevelTable])--�����ȼ���
    actorBase:SetLevel(level)--���ý�ɫ�ȼ� 
    actorBase:SetTimer(self:AnysisTimer(actorBase,actorConfig) )--ÿ����ɫ��������ʱ����ÿ����ʱ������һ������  
    actorBase:SetActorType(actorConfig.ActorType) --��ɫ����
    actorBase:SetBody(self:CreateBody(archID,pos,actorConfig.ActorType))
    self.ActorTable[actorBase:GetID()] = actorBase   
    
    return actorBase
end 
function ActorProxy:RemoveActor(actorID)  
    self.ActorTable[actorID]:Destory()  --ɾ���Լ���һЩ��Ϣ
    self.ActorTable[actorID] = nil
end
function ActorProxy:GetActor(actorID)
    return self.ActorTable[actorID]
end
function ActorProxy:GetActorListKey()
    return GetTableKey(self.ActorTable)
end 
return ActorProxy
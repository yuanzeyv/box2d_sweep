local BaseProxy = require("Logic.Proxy.BaseProxy")
local ActorAttributeProxy = class("ActorAttributeProxy",BaseProxy) 
function ActorAttributeProxy:InitData() 
    self.SystemList = {} --当前注册的系统
    self.SystemAttributeTable = {} --系统类型属性表（基础表 和 附加表）   
    self.SystemAttributeSumTable = {}--所有系统属性总和表
    self.SumAttribute = {} --计算属性总和表 (计算后的表，就只保留基础属性了)  
end 

function ActorAttributeProxy:LoadFinish() 
    self.ActorFacadeProxy = self:getFacade():retrieveProxy(ProxyTable.ActorFacadeProxy) 
    self:RegisterAttrSystem()
end  

function ActorAttributeProxy:ClearData() 
end 

function ActorAttributeProxy:RegisterAttrSystem()
    self:RegisterAttributeSystem(EnumDefine.AttributeSystem.LevelSystem)
end 
 
function ActorAttributeProxy:RegisterAttributeSystem(system)
    assert(not self.SystemList[system],"系统已经被注册过了") 
    self.SystemList[system] = true
end 

function ActorAttributeProxy:RegisterActor(actorID) 
    assert(not self.SystemAttributeTable[actorID],"角色重复注册") 
    self.SystemAttributeTable[actorID] = {} 
    self.SystemAttributeSumTable[actorID] = {} 
    self.SumAttribute[actorID] = {}   
end 

function ActorAttributeProxy:UnRegisterActor(actorID) 
    assert(self.SystemAttributeTable[actorID],"角色未被注册过了") 
    self.SystemAttributeTable[actorID] = nil
    self.SystemAttributeSumTable[actorID] = nil
    self.SumAttribute[actorID] = nil   
end 

--删除一个系统的附加表
function ActorAttributeProxy:DeleteSystemTable(actorID,systemType)  
    assert(self.SystemList[systemType],"系统不存在") 
    assert(self.SystemAttributeTable[actorID],"角色未被注册过了") 
    if not self.SystemAttributeTable[actorID][systemType] then return end--系统不存在的话
    local attrConfig = self.SystemAttributeTable[actorID][systemType]--取得属性表
    self.SystemAttributeTable[actorID][systemType] = nil --删除属性表
    for v,k in pairs(attrConfig) do 
        local index = EnumDefine.AttributeKey[v]
        if index then 
            local attrValue = self.SystemAttributeSumTable[index] or 0
            self.SystemAttributeSumTable[actorID][index] = attrValue - k
            local baseIndex = (index % 1001 + 1 )
            self.SumAttribute[actorID][baseIndex] = nil --清空总属性表的这个字段，之后会重新计算 
        end 
    end  
end

--添加一个系统的附加表
function ActorAttributeProxy:AdditionSystemTable(actorID,systemType,attrConfig)  
    assert(self.SystemList[systemType],"系统不存在") 
    assert(not self.SystemAttributeTable[actorID][systemType],"角色属性已经存在了")
    self.SystemAttributeTable[actorID][systemType] = attrConfig
    for v,k in pairs(attrConfig) do 
        local index = EnumDefine.AttributeKey[v]
        if index then  
            local attrValue = self.SystemAttributeSumTable[actorID][index] or 0
            self.SystemAttributeSumTable[actorID][index] = attrValue + k
            local baseIndex = (index % 1001 + 1 )
            self.SumAttribute[actorID][baseIndex] = nil --清空总属性表的这个字段，之后会重新计算 
        end  
    end   
end 
--注册一个角色属性
function ActorAttributeProxy:UpdateSystemTable(actorID,system,attrConfig) 
    assert(self.SystemList[system],"系统不存在")  
    if self.SystemAttributeTable[actorID][system] then ---如果系统中已经存在有了，删除当前属性
        self:DeleteSystemTable(actorID,system)
    end  
    self:AdditionSystemTable(actorID,system,attrConfig)  
    self.ActorFacadeProxy:RecalcActorCoreAttr(actorID)

end  

--计算属性 
function ActorAttributeProxy:CalcAttr(actorID,attrType)
    assert(self.SystemAttributeTable[actorID],"角色未被注册过了")  
    assert(attrType >= 0 and attrType < 1000 ,"只能计算基础属性")  
    local actorAttrTable = self.SystemAttributeSumTable[actorID]
    local baseAttr = actorAttrTable[EnumDefine.AttributeType.Base + attrType] or 0 --获取到基础属性
    local additionAttr = actorAttrTable[attrType + EnumDefine.AttributeType.Addition] or 0--获取到附加属性
    local BasePercent = actorAttrTable[attrType + EnumDefine.AttributeType.BasePercent] or 0--基础属性百分比加成
    local AdditionPercent = actorAttrTable[attrType + EnumDefine.AttributeType.AdditionPercent] or 0--附加属性百分比加成
    local SumPercent = actorAttrTable[attrType + EnumDefine.AttributeType.SumPercent] or 0--总属性加成 
    local sumValue = baseAttr * ( 1 + BasePercent)   + additionAttr * (1 + AdditionPercent)   
    return  (SumPercent + 1) * sumValue
end 
--获取到一个基础属性
function ActorAttributeProxy:GetAttrValue(actorID,attrType)  
    assert(attrType >= 0 and attrType < 1000 ,"只能取得基础属性:"..attrType) 
    local sumAttrTable = self.SumAttribute[actorID] 
    if not sumAttrTable[attrType] then 
        sumAttrTable[attrType] = self:CalcAttr(actorID,attrType)
    end
    return sumAttrTable[attrType]
end  
return ActorAttributeProxy
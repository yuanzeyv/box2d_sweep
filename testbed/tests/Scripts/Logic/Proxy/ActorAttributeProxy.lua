local BaseProxy = require("Logic.Proxy.BaseProxy")
local ActorAttributeProxy = class("ActorAttributeProxy",BaseProxy) 
function ActorAttributeProxy:InitData() 
    self.SystemList = {} --��ǰע���ϵͳ
    self.SystemAttributeTable = {} --ϵͳ�������Ա������� �� ���ӱ�   
    self.SystemAttributeSumTable = {}--����ϵͳ�����ܺͱ�
    self.SumAttribute = {} --���������ܺͱ� (�����ı���ֻ��������������)  
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
    assert(not self.SystemList[system],"ϵͳ�Ѿ���ע�����") 
    self.SystemList[system] = true
end 

function ActorAttributeProxy:RegisterActor(actorID) 
    assert(not self.SystemAttributeTable[actorID],"��ɫ�ظ�ע��") 
    self.SystemAttributeTable[actorID] = {} 
    self.SystemAttributeSumTable[actorID] = {} 
    self.SumAttribute[actorID] = {}   
end 

function ActorAttributeProxy:UnRegisterActor(actorID) 
    assert(self.SystemAttributeTable[actorID],"��ɫδ��ע�����") 
    self.SystemAttributeTable[actorID] = nil
    self.SystemAttributeSumTable[actorID] = nil
    self.SumAttribute[actorID] = nil   
end 

--ɾ��һ��ϵͳ�ĸ��ӱ�
function ActorAttributeProxy:DeleteSystemTable(actorID,systemType)  
    assert(self.SystemList[systemType],"ϵͳ������") 
    assert(self.SystemAttributeTable[actorID],"��ɫδ��ע�����") 
    if not self.SystemAttributeTable[actorID][systemType] then return end--ϵͳ�����ڵĻ�
    local attrConfig = self.SystemAttributeTable[actorID][systemType]--ȡ�����Ա�
    self.SystemAttributeTable[actorID][systemType] = nil --ɾ�����Ա�
    for v,k in pairs(attrConfig) do 
        local index = EnumDefine.AttributeKey[v]
        if index then 
            local attrValue = self.SystemAttributeSumTable[index] or 0
            self.SystemAttributeSumTable[actorID][index] = attrValue - k
            local baseIndex = (index % 1001 + 1 )
            self.SumAttribute[actorID][baseIndex] = nil --��������Ա������ֶΣ�֮������¼��� 
        end 
    end  
end

--���һ��ϵͳ�ĸ��ӱ�
function ActorAttributeProxy:AdditionSystemTable(actorID,systemType,attrConfig)  
    assert(self.SystemList[systemType],"ϵͳ������") 
    assert(not self.SystemAttributeTable[actorID][systemType],"��ɫ�����Ѿ�������")
    self.SystemAttributeTable[actorID][systemType] = attrConfig
    for v,k in pairs(attrConfig) do 
        local index = EnumDefine.AttributeKey[v]
        if index then  
            local attrValue = self.SystemAttributeSumTable[actorID][index] or 0
            self.SystemAttributeSumTable[actorID][index] = attrValue + k
            local baseIndex = (index % 1001 + 1 )
            self.SumAttribute[actorID][baseIndex] = nil --��������Ա������ֶΣ�֮������¼��� 
        end  
    end   
end 
--ע��һ����ɫ����
function ActorAttributeProxy:UpdateSystemTable(actorID,system,attrConfig) 
    assert(self.SystemList[system],"ϵͳ������")  
    if self.SystemAttributeTable[actorID][system] then ---���ϵͳ���Ѿ��������ˣ�ɾ����ǰ����
        self:DeleteSystemTable(actorID,system)
    end  
    self:AdditionSystemTable(actorID,system,attrConfig)  
    self.ActorFacadeProxy:RecalcActorCoreAttr(actorID)

end  

--�������� 
function ActorAttributeProxy:CalcAttr(actorID,attrType)
    assert(self.SystemAttributeTable[actorID],"��ɫδ��ע�����")  
    assert(attrType >= 0 and attrType < 1000 ,"ֻ�ܼ����������")  
    local actorAttrTable = self.SystemAttributeSumTable[actorID]
    local baseAttr = actorAttrTable[EnumDefine.AttributeType.Base + attrType] or 0 --��ȡ����������
    local additionAttr = actorAttrTable[attrType + EnumDefine.AttributeType.Addition] or 0--��ȡ����������
    local BasePercent = actorAttrTable[attrType + EnumDefine.AttributeType.BasePercent] or 0--�������԰ٷֱȼӳ�
    local AdditionPercent = actorAttrTable[attrType + EnumDefine.AttributeType.AdditionPercent] or 0--�������԰ٷֱȼӳ�
    local SumPercent = actorAttrTable[attrType + EnumDefine.AttributeType.SumPercent] or 0--�����Լӳ� 
    local sumValue = baseAttr * ( 1 + BasePercent)   + additionAttr * (1 + AdditionPercent)   
    return  (SumPercent + 1) * sumValue
end 
--��ȡ��һ����������
function ActorAttributeProxy:GetAttrValue(actorID,attrType)  
    assert(attrType >= 0 and attrType < 1000 ,"ֻ��ȡ�û�������:"..attrType) 
    local sumAttrTable = self.SumAttribute[actorID] 
    if not sumAttrTable[attrType] then 
        sumAttrTable[attrType] = self:CalcAttr(actorID,attrType)
    end
    return sumAttrTable[attrType]
end  
return ActorAttributeProxy
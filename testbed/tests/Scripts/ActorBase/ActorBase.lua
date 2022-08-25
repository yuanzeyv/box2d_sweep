--��Ϸ��ɫ�� ������ϵͳ������ģ����ע�ᣬ��ģ���б����Ž�ɫ���������� 
--һ����ɫ���������ԣ�����ײ���ж�ʱ��
local ActorBase = class("ActorBase")  
function ActorBase:ctor(...)
    self:InitData()
end 
function ActorBase:Destory()
    self:CloseTimer()--�ر����ж�ʱ��
    sol.BodyMan.Instance():DeleteBody(self.ID)--ɾ���Լ� 
end 
function ActorBase:InitData()  --��������Ϣ���뵽��ɫ  
    self.BodyData = nil 
    self.LevelTable = nil 
    self.ID = nil 
    self.ActorType = nil
    self.Level = nil
    self.CoreAttr = {}  
    self.TimerList = {} --Ĭ�ϻ���ҷ���һ�� ��ʱ��������������  
    self.ActorFacadeProxy = G_FACADE:retrieveProxy(ProxyTable.ActorFacadeProxy)  
    self:CoreAttrSetZero()

    self.ActionBits = 0
end

function ActorBase:CoreAttrSetZero()
    self.CoreAttr.Exp = 0
    self.CoreAttr.HP = 0
    self.CoreAttr.MP =0 
end 

function ActorBase:ReCalcCoreAttr() 
    local MaxHP = self.ActorFacadeProxy:GetAttrValue(self.ID,EnumDefine.AttributeKey.HP)
    local MaxMP = self.ActorFacadeProxy:GetAttrValue(self.ID,EnumDefine.AttributeKey.MP) 
    local MaxExp = self.ActorFacadeProxy:GetAttrValue(self.ID,EnumDefine.AttributeKey.UpLevelExp) 
    self.CoreAttr.HP = (self.CoreAttr.HP > MaxHP) and MaxHP or self.CoreAttr.HP;
    self.CoreAttr.MP = (self.CoreAttr.MP > MaxMP) and MaxMP or self.CoreAttr.MP;
    self.CoreAttr.Exp = (self.CoreAttr.HP > MaxExp) and MaxExp or self.CoreAttr.Exp;  
end 

function ActorBase:InitCoreAttr()
    self.CoreAttr.Exp = 0
    self.CoreAttr.HP = self.ActorFacadeProxy:GetAttrValue(self.ID,EnumDefine.AttributeKey.HP)
    self.CoreAttr.MP = self.ActorFacadeProxy:GetAttrValue(self.ID,EnumDefine.AttributeKey.MP)
end 
function ActorBase:GetCoreAttr()
    return self.CoreAttr
end  

function ActorBase:SetLevelTable(levelTable)
    self.LevelTable = levelTable
end  

function ActorBase:GetLevelConfig()
    return self.LevelTable[self.Level]
end  

function ActorBase:SetLevel(level)
    self.Level = level
end
function ActorBase:GetLevel()
    return self.Level
end   
--��ȡ��Body
function ActorBase:GetBody()
    return self.Body
end  
function ActorBase:SetBody(body)
    self.BodyData = body
    self.Body = body.Body
    self.ID = body.ID
end 

--��ȡ��ID
function ActorBase:GetID()
    return self.ID
end 

function ActorBase:SetTimer(timeList)
    self.TimerList = timeList
end 

function ActorBase:OpenTimer(index) 
    index = (index and index) or 0
    local OperList = {[0] = {1,2,3,},[1] = {1} ,[2]={2},[3]={3}}   
    for v,k in pairs(OperList) do 
        if self.TimerList[v] then  
            self.TimerList[v]:OpenTimer() 
        end
    end  
end 
function ActorBase:CloseTimer(index)
    index = (index and index) or 0
    local OperList = {[0] = {1,2,3,},[1] = {1} ,[2]={2},[3]={3}}    
    for v,k in pairs(OperList[index] or {} ) do 
        if self.TimerList[v] then  
            self.TimerList[v]:CloseTimer() 
        end  
    end  
end  
   
--��ȡ����ײ��Ϣ
function ActorBase:GetColliderConfig(colliderType)
    local mappingTable = {} 
    mappingTable[EnumDefine.ColliderType.BeginContact] = "BeginCollider"
    mappingTable[EnumDefine.ColliderType.EndContact] = "EndCollider" 
    mappingTable[EnumDefine.ColliderType.PostSolve] = "ContinueCollider"  
    return self.BodyData.ModelTriggerTable[mappingTable[colliderType]]
end 
--�ȼ�����
function ActorBase:LevelUP()
    --self.LevelTable[] �Խ�ɫ�ĵȼ���������
end 
 
--��ȡ����ǰ������
function ActorBase:GetAttr(type)
    return self.ActorFacadeProxy:GetAttrValue(self.ID,type)
end  

--��ȡ����ǰ�Ľ�ɫ��������
function ActorBase:GetActionBits(bits)
    bits = assert(tonumber(bits),"��ǰҪ��ѯ���ֽڲ�������") 
    return self.ActionBits & bits --����Ҫ��ѯ���ֶ�
end 

--��ȡ����ǰ�Ľ�ɫ��������
function ActorBase:SetActionBits(offset,value)  
    if value == 0 then  
        self.ActionBits = self.ActionBits ~ offset  
    else 
        self.ActionBits = self.ActionBits | offset  
    end   
end   

--���ý�ɫ����
function ActorBase:SetActorType(type) --��ɫ����
    self.ActorType = type
end 
function ActorBase:GetActorType(type) --��ɫ����
    return self.ActorType
end 
--��ȡ��ɫ����
return ActorBase
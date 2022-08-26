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

--ͨ����ͼ����һ����ɫ
function ActorFacadeProxy:CreateActor_GameMap(gameMapID)
    local mapCellConfig = assert(self.MapProxy:GetMapTable(gameMapID),"Ҫ�����ĵ�Ԫ������ ") 
    return self:CreateActor(mapCellConfig.MonsterModel,mapCellConfig.ActorID,SplitB2Vec2(mapCellConfig.POS),mapCellConfig.MonsterLevel) 
end
--ֱ�Ӵ���һ����ɫ
function ActorFacadeProxy:CreateActor(modelID,actorID,pos,level)
    modelID = assert(tonumber(modelID),"ģ��IDΪ��")
    actorID = assert(tonumber(actorID),"��ɫIDΪ��") 
    assert(pos,"λ��Ϊ��")
    level =  level or 1 
    local ActorBase = self.ActorProxy:CreateActor(actorID,modelID,pos,level)--����һ����ɫ����ʱ��ע��  
    local ID = ActorBase:GetID()
    self.ActorAttributeManagerProxy:RegisterActor(ID)--����ɫ����������ȥ 
    self.ActorAttributeManagerProxy:UpdateSystemTable(ID,EnumDefine.AttributeSystem.LevelSystem,ActorBase:GetLevelConfig())--����ɫ����������ȥ   
    self.ActorBehaivorProxy:RegisterPlayer(ActorBase) 
    ActorBase:InitCoreAttr() 
    ActorBase:OpenTimer()    
    return ActorBase  
end
--ɾ��һ����ɫ 
function ActorFacadeProxy:DestoryActor(ID)
    assert(ID,"����Ľ�ɫIDΪ��")  
    ID = tonumber(ID)
    self.ActorProxy:RemoveActor(ID)--ɾ����ɫ��Ϣ
    self.ActorAttributeManagerProxy:UnRegisterActor(ID)--ɾ��������Ϣ 
end 

--��ȡ��һ����ɫ
function ActorFacadeProxy:GetActor(actorID)
    return self.ActorProxy:GetActor(actorID)
end  

--��ȡ��һ����ɫ������
function ActorFacadeProxy:GetAttrValue(actorID,attrType)
    return self.ActorAttributeManagerProxy:GetAttrValue(actorID,attrType) 
end  

--��ȡ��һ����ɫ������
function ActorFacadeProxy:RecalcActorCoreAttr(actorID)
    local actor = self.ActorProxy:GetActor(actorID) 
    if not actor then return end 
    actor:ReCalcCoreAttr()
end 
 

--��ʼ����ɫ�ĺ�������
function ActorFacadeProxy:ClearData(actorID)
    local keyTable = self.ActorProxy:GetActorListKey() 
    for v,k in pairs(keyTable) do 
        self:DestoryActor(k) 
    end  
end 
return ActorFacadeProxy
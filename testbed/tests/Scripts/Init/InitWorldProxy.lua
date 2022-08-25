local InitWorldProxy = class("InitWorldProxy") 
function InitWorldProxy:ctor()  
    self.BasePath = "Logic.Proxy." 
    self.ProxyPathTable = {} 
    self:InitProxyTable()
end 
function InitWorldProxy:InitProxyTable()   
    self:RegisterProxy("ActorAttributeProxy")--��ײ����
    self:RegisterProxy("MapProxy" )--��ͼ���� 
    self:RegisterProxy("ColliderProxy")--��ײ����
    self:RegisterProxy("TriggerProxy")--��ײ����  
    self:RegisterProxy("ActorProxy")--��ɫ����
    self:RegisterProxy("ActorAttributeProxy")--��ɫװ����
    self:RegisterProxy("ActorFacadeProxy")--��ɫװ����  
    self:RegisterProxy("ActorBehaivorProxy")--��ɫ��Ϊ������
    self:RegisterProxy("ActorMoveProxy")--��ɫ�ƶ�����
    self:RegisterProxy("NavmeshManagerProxy")--Ѱ·����
end 
function InitWorldProxy:RegisterProxy(path)  
    table.insert( self.ProxyPathTable,self.BasePath .. path ) 
end 
function InitWorldProxy:InitProxy()
    local ProxyList = {} 
    for index,path in pairs(self.ProxyPathTable) do
        local ProxyBase = require(path).new()
        local ProxyName = ProxyBase:getProxyName()
        ProxyTable[ProxyName] = ProxyName
        G_FACADE:registerProxy(ProxyBase) 
        table.insert( ProxyList,ProxyBase )
    end 
    for index,Proxy in pairs(ProxyList) do
        Proxy:LoadFinish()
    end  
end 
return InitWorldProxy    


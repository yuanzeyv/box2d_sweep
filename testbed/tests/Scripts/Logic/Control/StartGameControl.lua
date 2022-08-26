local BaseControl = require("Logic.Control.BaseControl")
local StartGameControl = class("StartGameControl",BaseControl)  
--�������ݵĳ�ʼ��
function StartGameControl:InitData() 
    self.MapProxy = self:getFacade():retrieveProxy(ProxyTable.MapProxy) 
    self.ActorFacadeProxy = self:getFacade():retrieveProxy(ProxyTable.ActorFacadeProxy) 
    self.NavmeshManagerProxy = self:getFacade():retrieveProxy(ProxyTable.NavmeshManagerProxy) 
end  

function StartGameControl:Execute(notification)  
    self.MapProxy:CreateMap()--�������ŵ�ͼ   
    G_FACADE:sendNotification(NotifyDefine.StaticBodyExportControl) --������̬����
    self.NavmeshManagerProxy:GenerateNavmesh("a",self.MapProxy:GetMapID())    --�������ŵ�ͼ
end 
return StartGameControl
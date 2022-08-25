local BaseControl = require("Logic.Control.BaseControl")
local StartGameControl = class("StartGameControl",BaseControl)  
--用于数据的初始化
function StartGameControl:InitData() 
    self.MapProxy = self:getFacade():retrieveProxy(ProxyTable.MapProxy) 
    self.ActorFacadeProxy = self:getFacade():retrieveProxy(ProxyTable.ActorFacadeProxy) 
    self.NavmeshManagerProxy = self:getFacade():retrieveProxy(ProxyTable.NavmeshManagerProxy) 
end  

function StartGameControl:Execute(notification)  
    self.MapProxy:CreateMap()--创建这张地图   
    G_FACADE:sendNotification(NotifyDefine.StaticBodyExportControl) --导出静态部分
    self.NavmeshManagerProxy:GenerateNavmesh("a",self.MapProxy:GetMapID())    --编译这张地图
    Actor = self.ActorFacadeProxy:CreateActor(2,2,sol.b2Vec2.new(0,0),1) --创建一个建筑ID为2的玩家   
    Player = Actor:GetBody() 
    self.NavmeshManagerProxy:RegisterActor(Actor) 
end 
return StartGameControl
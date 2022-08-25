local BaseControl = require("Logic.Control.BaseControl")
local ResourceLoad = class("ResourceLoad",BaseControl)  
--用于数据的初始化
function ResourceLoad:InitData() 
    self.MapProxy = self:getFacade():retrieveProxy(ProxyTable.MapProxy) 
    self.ActorFacadeProxy = self:getFacade():retrieveProxy(ProxyTable.ActorFacadeProxy) 
end  

function ResourceLoad:Execute(notification)  
    --加载到物理建筑
    sol.PhysicsParse.ParsePhysicsParse("tests/Resource/ColliderFile/vvv.xml");  
    sol.PhysicsParse.ParsePhysicsParse("tests/Resource/ColliderFile/Player.xml");  
end 
return ResourceLoad 
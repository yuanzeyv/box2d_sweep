local BaseMediator = require("Logic.Mediator.BaseMediator")
local PlayerCreateMediator = class("PlayerCreateMediator",BaseMediator)   
function PlayerCreateMediator:InitData() 
    self.ActorMoveProxy =G_FACADE:retrieveProxy(ProxyTable.ActorMoveProxy) --角色移动代理
    self.ActorBehaivorProxy =G_FACADE:retrieveProxy(ProxyTable.ActorBehaivorProxy) --角色移动代理
    self.ActorFacadeProxy =G_FACADE:retrieveProxy(ProxyTable.ActorFacadeProxy) --角色移动代理
    
end   
function PlayerCreateMediator:KeyInput_Handle(data) 
    if data.pushKey == "9" then      
        sol.PlayerMan.Instance():RegisterPlayer("yuan","1111","gudu")
    end  
end

function PlayerCreateMediator:NotifycationRegister(table) 
    table[NotifyDefine.KeyInput] = handler(self,self.KeyInput_Handle)
end 

--对象被销毁的时候要调用的函数
function PlayerCreateMediator:ClearData() 
end 
return  PlayerCreateMediator 
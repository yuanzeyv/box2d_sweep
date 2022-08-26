local BaseProxy = require("Logic.Proxy.BaseProxy")
local PlayerManagerProxy = class("PlayerManagerProxy",BaseProxy)   
function PlayerManagerProxy:InitData()
end  
 
function PlayerManagerProxy:LoadFinish()  
    self.NavmeshManagerProxy = self:getFacade():retrieveProxy(ProxyTable.NavmeshManagerProxy)
    self.ActorFacadeProxy = self:getFacade():retrieveProxy(ProxyTable.ActorFacadeProxy) 
end      
--我期望的结果是，如果向移动，客户端每秒钟像我发送一次移动状态刷新。
function PlayerManagerProxy:Step(time) --设置角色的移动位置
    local playerData = sol.PlayerMan.Instance():PoPWaitPlayer()
    if not playerData then 
        return 
    end  
    --开始创建这个角色 
    local actor = self.ActorFacadeProxy:CreateActor(2,2,sol.b2Vec2.new(0,0),1) --创建一个建筑ID为2的玩家     
    self.NavmeshManagerProxy:RegisterActor(actor)  
    playerData:SetBodyData(actor:GetBody())  
    sol.PlayerMan.Instance():PlayerEnterGame(playerData) 
end
 
return PlayerManagerProxy 
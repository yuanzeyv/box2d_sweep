local InitWorldProxy = class("InitWorldProxy") 
function InitWorldProxy:ctor()  
    self.BasePath = "Logic.Proxy." 
    self.ProxyPathTable = {} 
    self:InitProxyTable()
end 
function InitWorldProxy:InitProxyTable()   
    self:RegisterProxy("ActorAttributeProxy")--碰撞管理
    self:RegisterProxy("MapProxy" )--地图管理 
    self:RegisterProxy("ColliderProxy")--碰撞管理
    self:RegisterProxy("TriggerProxy")--碰撞管理  
    self:RegisterProxy("ActorProxy")--角色管理
    self:RegisterProxy("ActorAttributeProxy")--角色装饰器
    self:RegisterProxy("ActorFacadeProxy")--角色装饰器  
    self:RegisterProxy("ActorBehaivorProxy")--角色行为树代理
    self:RegisterProxy("ActorMoveProxy")--角色移动代理
    self:RegisterProxy("NavmeshManagerProxy")--寻路代理
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


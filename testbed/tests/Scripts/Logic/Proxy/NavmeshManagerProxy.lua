local BaseProxy = require("Logic.Proxy.BaseProxy")
local NavmeshManagerProxy = class("NavmeshManagerProxy",BaseProxy)   
--用于管理游戏中的寻路
function NavmeshManagerProxy:InitData()
   self.NavMeshTable = {} --管理注册过的navmesh
end 
--当一个角色准备使用
function NavmeshManagerProxy:LoadFinish()   
end   
--将角色加入到寻路地图
function NavmeshManagerProxy:RegisterActor(actor)
    sol.NavmeshMan.Instance():RegisterActor(self.NavMeshTable["AAA"],actor:GetID())  --祖册当前的角色
end 
--加载一个地图
function NavmeshManagerProxy:GenerateNavmesh(path,mapID) --对应的地图ID
    local navMeshID = sol.NavmeshMan.Instance():GenerateNavmesh(path)   
    self.NavMeshTable[mapID] = navMeshID--目前地图就一个
end     
function NavmeshManagerProxy:Recast(actor, endPoint)
    print("当前的初始位置为:",actor:GetBody():GetPosition().x,actor:GetBody():GetPosition().y)
    return sol.NavmeshMan.Instance():Recast(actor:GetID(),actor:GetBody():GetPosition(),endPoint)   
end
return NavmeshManagerProxy
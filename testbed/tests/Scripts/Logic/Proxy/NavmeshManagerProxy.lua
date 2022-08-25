local BaseProxy = require("Logic.Proxy.BaseProxy")
local NavmeshManagerProxy = class("NavmeshManagerProxy",BaseProxy)   
--���ڹ�����Ϸ�е�Ѱ·
function NavmeshManagerProxy:InitData()
   self.NavMeshTable = {} --����ע�����navmesh
end 
--��һ����ɫ׼��ʹ��
function NavmeshManagerProxy:LoadFinish()   
end   
--����ɫ���뵽Ѱ·��ͼ
function NavmeshManagerProxy:RegisterActor(actor)
    sol.NavmeshMan.Instance():RegisterActor(self.NavMeshTable["AAA"],actor:GetID())  --��ᵱǰ�Ľ�ɫ
end 
--����һ����ͼ
function NavmeshManagerProxy:GenerateNavmesh(path,mapID) --��Ӧ�ĵ�ͼID
    local navMeshID = sol.NavmeshMan.Instance():GenerateNavmesh(path)   
    self.NavMeshTable[mapID] = navMeshID--Ŀǰ��ͼ��һ��
end     
function NavmeshManagerProxy:Recast(actor, endPoint)
    print("��ǰ�ĳ�ʼλ��Ϊ:",actor:GetBody():GetPosition().x,actor:GetBody():GetPosition().y)
    return sol.NavmeshMan.Instance():Recast(actor:GetID(),actor:GetBody():GetPosition(),endPoint)   
end
return NavmeshManagerProxy
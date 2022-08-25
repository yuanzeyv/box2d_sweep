local BaseProxy = require("Logic.Proxy.BaseProxy")
local MapProxy = class("MapProxy",BaseProxy)   
function MapProxy:InitData()
   self.GameMap =  require "Config.GameMapTable"--��ʼ����ͼ�� 
   self.MapBodyTable = {} 
   self.MapID = "AAA"
end 
function MapProxy:GetMapID()
   return self.MapID
end 

function MapProxy:LoadFinish() 
   self.ActorFacadeProxy = self:getFacade():retrieveProxy(ProxyTable.ActorFacadeProxy)  
end   

function MapProxy:CreateMap()--��ʼ���������ͼ  
   for v,k in pairs(self.GameMap) do 
      local actor = self.ActorFacadeProxy:CreateActor_GameMap(v) --����������������֮�� 
      self.MapBodyTable[actor:GetID()]  = actor
   end    
end  
function MapProxy:GetMapTable(id)
   return self.GameMap[id]
end  
return MapProxy
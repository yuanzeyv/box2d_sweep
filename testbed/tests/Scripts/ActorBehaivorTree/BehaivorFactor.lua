local BehaivorBase = class("BehaivorBase")
local CoroActionNode = require("ActorBehaivorTree.CoroActionNode")
function BehaivorBase:ctor(xml)  
    self:__InitData(xml) 
end 
--用于数据的初始化
function BehaivorBase:__InitData(xml) 
    self.BehaviorFactor = sol.BTFactory.new() --创建一个工厂 
    self:BehaviorInit() --初始化行为树
    self.BehaviorFactor:registerBehaviorTreeFromText(xml)
end   
function BehaivorBase:RegisterCoroAction(id,handle,port)
    local safeFunc = function(...) 
        local status,result = xpcall(handle, __TRACKBACK__,...)   
        return result
    end  
    local CoroObj  = CoroActionNode.new(safeFunc) 
    self.BehaviorFactor:RegisterCoroAction(id, CoroObj,port or {} );
end  

function BehaivorBase:RegisterConditionAction(id,handle,port)
    local safeFunc = function(...)  
        local status,result = xpcall(handle, __TRACKBACK__,...)   
        return result
    end  
    self.BehaviorFactor:RegisterConditionAction(id,safeFunc,port or {} ); --初始化sayMsg方法 
end

--初始化内部函数
function BehaivorBase:BehaviorInit()
end    

function BehaivorBase:CreateTree(param) 
    local tree = self.BehaviorFactor:createTree()
    tree:InitLuaNodeParam(param)
    return tree
end 
return BehaivorBase  
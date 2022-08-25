local CoroActionNode = class("CroActionNode")
function CoroActionNode:ctor(handle)
    self:__InitData(handle)
end 

function CoroActionNode:__InitData(handle) 
    self.TickFunc = handle
end 
function CoroActionNode:CoroFunc(obj,paramTable) 
    --如果当前还没有的话创建一个协程
    if not obj.ThreadID  then 
        obj.ThreadID = coroutine.create(self.TickFunc)
    end  
    local ret,result = coroutine.resume(obj.ThreadID,obj,paramTable)
    if not ret then--如果函数运行错误的话 
        self:HaltCoro(obj) --关闭当前协层 
        return EnumDefine.NodeStatus.FAILURE
    end
    if coroutine.status(obj.ThreadID ) == "dead" then --当前协程已经运行完毕
        self:HaltCoro(obj) --关闭当前协层 
    end
    return result
end  

function CoroActionNode:HaltCoro(obj)
    if self.ThreadID then 
        coroutine.close(obj.ThreadID)
        obj.ThreadID = nil
    end 
end 
return CoroActionNode
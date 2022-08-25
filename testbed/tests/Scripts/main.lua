

require "Init.Init"    
G_Step = 0 
Main = function ()
    local  Safe_Main = function ()  
       Init() --进行初始化    
    end   
    xpcall(Safe_Main, __TRACKBACK__) 
end   

Tick = function (time)  
    local Safe_Tick = function (time) 
        local ActorBehaivorProxy = G_FACADE:retrieveProxy(ProxyTable.ActorBehaivorProxy)  
        ActorBehaivorProxy:Step(time)   
        G_Step = 0
    end 
    xpcall(Safe_Tick, __TRACKBACK__,time) 
end
Exit = function ()  
    local  Safe_Exit = function () 
        G_FACADE.removeCore(G_FACADE.multitonKey) 
    end
    xpcall(Safe_Exit, __TRACKBACK__) 
end 
Input = function (key)  
    local Safe_Input = function (key)
        G_FACADE:sendNotification(NotifyDefine.KeyInput,{actorID = Player:ID(),pushKey = key})  
    end
    xpcall(Safe_Input, __TRACKBACK__,key) 
end 
Main()
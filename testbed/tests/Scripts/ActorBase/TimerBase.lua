local TimerBase = class("TimerBase")  
function TimerBase:ctor(...)
    self:InitData(...)
end 
function TimerBase:InitData(actor,delay,isLoop,executeList)  
    local TimeTable = {}  
    self.IsLoop = tonumber(isLoop)
    self.Delay = tonumber(delay)
    self.ExecuteList = executeList
    self.Actor = actor
    self.m_TimeObj = sol.LuaTimeEvecnt:new(self) 
    
    self.TriggerProxy = G_FACADE:retrieveProxy(ProxyTable.TriggerProxy)   
end

function TimerBase:Execute(timeObj) 
    local  Execute = function ()   
        self.TriggerProxy:ExecuteTrigger(self.Actor,self.ExecuteList) --执行  
        if self.IsLoop == 1 then  
            self:OpenTimer()
        end  
    end
    xpcall(Execute, __TRACKBACK__)  
end 

function TimerBase:OpenTimer()  
    if self.m_TimeObj:active() then--定时器被弹出时,已经清空了数据,可以直接服用   
        print("当前定时器正在被打开中")
        return 
    end
    sol.TimeWheelMan.Instance():AddTime(self.m_TimeObj,self.Delay)
end 
function TimerBase:CloseTimer()
    if not self.m_TimeObj:active() then 
        print("定时器未开启")
        return 
    end 
    self.m_TimeObj:cancel()--取消使用当前定时器
end  
return TimerBase
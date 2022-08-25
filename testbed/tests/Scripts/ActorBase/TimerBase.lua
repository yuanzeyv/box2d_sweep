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
        self.TriggerProxy:ExecuteTrigger(self.Actor,self.ExecuteList) --ִ��  
        if self.IsLoop == 1 then  
            self:OpenTimer()
        end  
    end
    xpcall(Execute, __TRACKBACK__)  
end 

function TimerBase:OpenTimer()  
    if self.m_TimeObj:active() then--��ʱ��������ʱ,�Ѿ����������,����ֱ�ӷ���   
        print("��ǰ��ʱ�����ڱ�����")
        return 
    end
    sol.TimeWheelMan.Instance():AddTime(self.m_TimeObj,self.Delay)
end 
function TimerBase:CloseTimer()
    if not self.m_TimeObj:active() then 
        print("��ʱ��δ����")
        return 
    end 
    self.m_TimeObj:cancel()--ȡ��ʹ�õ�ǰ��ʱ��
end  
return TimerBase
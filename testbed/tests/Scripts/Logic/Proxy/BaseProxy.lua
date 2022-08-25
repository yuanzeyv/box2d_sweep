local BaseProxy = class("BaseProxy",framework.Proxy)
function BaseProxy:ctor() 
    framework.Proxy.ctor(self)
    self:InitData()
end 
function BaseProxy:onRegister()
    self:InitData()
end 
function BaseProxy:onRemove()
    self:ClearData()
end 
--用于数据的初始化
function BaseProxy:InitData() end 
--加载完成后，调用
function BaseProxy:LoadFinish() end 
--被清理是
function BaseProxy:ClearData()  end  
--用于调用其中的某个函数
function BaseProxy:Call(funcName,...)
    if not self[funcName] then return end  
    return self[funcName](...)
end 
return BaseProxy  
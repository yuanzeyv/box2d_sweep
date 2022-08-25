require "Tool.Class" --首先初始化工具类
local function InitGlobalFiled()
    require "MVCFrame.init"
    require "Tool.EnumDefine"
    require "Tool.Tool"
    require "Init.NotifyDefine"

    ProxyTable = {} 
end 
--打开服务器后，第一个进入的函数
function Init() 
    InitGlobalFiled() 
    G_FACADE = framework.Facade.getInstance(framework.FRAMEWORK_NAME)--初始化MVC框架 
    local WorldControl = require("Init.InitWorldControl").new()
    local WorldProxy = require("Init.InitWorldProxy").new()
    local WorldMediator = require("Init.InitWorldMediator").new() 
    WorldProxy:InitProxy()--初始化视图
    WorldMediator:InitMediator()--初始化代理 
    WorldControl:InitControl()--初始化控制器
    --初始化完成，发送开始消息
    G_FACADE:sendNotification(NotifyDefine.ResourceLoad) 
    G_FACADE:sendNotification(NotifyDefine.StartGameControl) 
    --加载这个导出的navmesh
end 
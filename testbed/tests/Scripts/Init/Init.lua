require "Tool.Class" --���ȳ�ʼ��������
local function InitGlobalFiled()
    require "MVCFrame.init"
    require "Tool.EnumDefine"
    require "Tool.Tool"
    require "Init.NotifyDefine"

    ProxyTable = {} 
end 
--�򿪷������󣬵�һ������ĺ���
function Init() 
    InitGlobalFiled() 
    G_FACADE = framework.Facade.getInstance(framework.FRAMEWORK_NAME)--��ʼ��MVC��� 
    local WorldControl = require("Init.InitWorldControl").new()
    local WorldProxy = require("Init.InitWorldProxy").new()
    local WorldMediator = require("Init.InitWorldMediator").new() 
    WorldProxy:InitProxy()--��ʼ����ͼ
    WorldMediator:InitMediator()--��ʼ������ 
    WorldControl:InitControl()--��ʼ��������
    --��ʼ����ɣ����Ϳ�ʼ��Ϣ
    G_FACADE:sendNotification(NotifyDefine.ResourceLoad) 
    G_FACADE:sendNotification(NotifyDefine.StartGameControl) 
    --�������������navmesh
end 
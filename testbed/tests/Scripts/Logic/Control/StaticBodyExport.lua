local BaseControl = require("Logic.Control.BaseControl")
local StaticBodyExport = class("StaticBodyExport",BaseControl)  
function StaticBodyExport:GetRealShape(fixture)
    local TypeFuncMap = {[EnumDefine.b2ShapeType.Circle] ="GetRealCircle" ,[EnumDefine.b2ShapeType.Polygon] = "GetRealPolygon"} 
    local shapeType = fixture:GetType() 
    if not TypeFuncMap[shapeType] then return false end 
    local shape = fixture:GetShape()--ѭ����ȡ����ǰ�����е����е���״
    shape = shape[TypeFuncMap[shapeType]](shape) 
    return shape--��ȡ������״ 
end 
function StaticBodyExport:GetStaticShapes()--��ȡ����ǰ��̬��ͼ�ϵ����ж�����Ϣ
    local i = 1 
    local shapesList = {} --��������,ÿ����״����һ����������
    local staticBodyTable = sol.BodyMan.Instance():GetBodyList(EnumDefine.BodyType.StaticBody)--��ȡ����ǰ��ͼ�ϵ����н�������
    for v,bodyData in pairs(staticBodyTable) do 
        local body = bodyData:GetBody() --���Ȼ�ȡ���İ�ǰ�ĸ��� 
        local bodyPos = body:GetPosition()--��̬����������ڸ��������
        local bodyRotate = body:GetAngle()--��ȡ�Ƕ�
        local rotateRad = math.rad(bodyRotate)--��ȡ����
        local fixture = body:GetFixtureList()--��ȡ���мо���Ϣ
        while(fixture) do--ѭ�������о� 
            local shape = self:GetRealShape(fixture)
            local points = nil
            if not shape then  
                goto Next
            end 
            points = shape:GetVectors(30) --Բ�β���Ҫ�������,��ֵԽ��,��״ԽԲ  
            i=i+ 1
            for index,vec in pairs(points) do    
                vec.x = vec.x + bodyPos.x
                vec.y = vec.y + bodyPos.y  
                vec.x= (vec.x - bodyPos.x)*math.cos(rotateRad)- (vec.y - bodyPos.y)*math.sin(rotateRad) + bodyPos.x ;
                vec.y= (vec.x - bodyPos.x)*math.sin(rotateRad)+ (vec.y - bodyPos.y)*math.cos(rotateRad) + bodyPos.y ;
                vec.x = vec.x 
                vec.y = -vec.y
            end 
            table.insert(shapesList,points)
            ::Next:: 
            fixture = fixture:GetNext() 
        end 
    end   
    return shapesList
end 

function StaticBodyExport:GetAABBBox(shapes)--��ȡ����ǰ��̬��ͼ�ϵ����ж�����Ϣ
    local leftLowerPos = sol.b2Vec2.new(999999,999999)
    local rightTopPos = sol.b2Vec2.new(-999999,-999999) 
    local rightLowerPos = sol.b2Vec2.new(0,0)
    local leftTopPos = sol.b2Vec2.new(0,0)
    for v,k in pairs(shapes) do --��ȡ������ 
        for _,point in pairs(k) do --��ȡ��ӵ�ÿ����
            local xPos = point.x
            local yPos = point.y
            if xPos < leftLowerPos.x then 
                leftLowerPos.x = xPos - 1
                leftTopPos.x = leftLowerPos.x 
            end 
            if yPos < leftLowerPos.y then 
                leftLowerPos.y = yPos - 1
                rightLowerPos.y = leftLowerPos.y
            end 
            if  xPos > rightTopPos.x then 
                rightTopPos.x = xPos + 1 
                rightLowerPos.x = rightTopPos.x
            end 
            if  yPos > rightTopPos.y then 
                rightTopPos.y = yPos + 1
                leftTopPos.y = rightTopPos.y
            end   
        end
    end
    return {leftLowerPos,leftTopPos,rightTopPos,rightLowerPos}
end
--��������ļ�
function StaticBodyExport:OutPutDelabellaFile(fileName,statisShapes,groundPoint)
    local indexTable = {} --��������
    local valueTable = {} --���ݱ�,���ڶ��㸴��
    local edgeTable = {} --�߱�
    local index = 0 --�±����㿪ʼ
    for v,k in pairs(statisShapes) do --ѭ���������е���״
        for _,point in pairs(k) do --��ȡ��ӵ�ÿ���� 
            if not valueTable[point.x .. ":" .. point.y] then --�����ǰû��ָ����index�Ļ�
                valueTable[point.x .. ":" .. point.y]  = index --Ϊ��ǰ��λ��ֵ 
                indexTable[index + 1] = point 
                index = index + 1 
            end
        end
        for i = 1,#k,1 do
            local point = k[i] 
            local nextPoint = k[ (i % #k) + 1 ] 
            edgeTable[#edgeTable + 1 ] = {valueTable[point.x .. ":" .. point.y],valueTable[nextPoint.x .. ":" .. nextPoint.y]}
        end  
    end
    for v,point in pairs(groundPoint)  do 
        if not valueTable[point.x .. ":" .. point.y] then --�����ǰû��ָ����index�Ļ�
            valueTable[point.x .. ":" .. point.y]  = index --Ϊ��ǰ��λ��ֵ 
            indexTable[index + 1] = point 
            index = index + 1 
        end
    end 
    file=io.open(fileName,"w")
    file:write((#indexTable ) .. " " .. #edgeTable.."\n") 
    for i = 1,#indexTable  do 
        file:write(indexTable[i].x .. " " ..indexTable[i].y.."\n")
    end   
    for v,k in pairs(edgeTable) do 
        file:write(k[1].. " " .. k[2].."\n")
    end 
    file:close()
end 
--�������ݵĳ�ʼ��
function StaticBodyExport:InitData()   
    local statisShapes = self:GetStaticShapes()--��ȡ�����о�̬��״
    local groundPoint = self:GetAABBBox(statisShapes)--��ȡ����Ե����  
    self:OutPutDelabellaFile("./a.txt",statisShapes,groundPoint)--��������ļ�����
    sol.Delabella.Generate3DMap("./a",false)--��ɺ�,���ýӿ�,��������ͼ��
end  

function StaticBodyExport:Execute(notification)   
end 
return StaticBodyExport
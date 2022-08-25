function clone(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        local newObject = {}
        lookup_table[object] = newObject
        for key, value in pairs(object) do
            newObject[_copy(key)] = _copy(value)
        end
        return setmetatable(newObject, getmetatable(object))
    end
    return _copy(object)
end 
function setMetaTableIndex(t, index)
    if t == nil or index == nil then
        return
    end
    local mt = getmetatable(t)
    if not mt then
        mt = {}
    end
    if not mt.__index then
        mt.__index = index
        setmetatable(t, mt)
    elseif mt.__index ~= index then
        setMetaTableIndex(mt, index)
    end
end  

function class(classname, ...) 
    local cls = { __cname = classname }
    local supers = { ... }
    for _, super in ipairs(supers) do
        if  type(super) == "table" then
            cls.__supers = cls.__supers or {}
            cls.__supers[#cls.__supers + 1] = super
            if not cls.super then
            end
            cls.super = super
        else
            return
        end
    end

    cls.__index = cls
    if not cls.__supers or #cls.__supers == 1 then
        setmetatable(cls, { __index = cls.super })
    else
        setmetatable(cls, { __index = function(_, key)
            local supers = cls.__supers
            for i = 1, #supers do
                local super = supers[i]
                if super[key] then
                    return super[key]
                end
            end
        end })
    end

    if not cls.ctor then
        cls.ctor = function()
        end
    end
    cls.new = function(...)
        local instance = {}
        setMetaTableIndex(instance, cls)
        instance.class = cls
        instance:ctor(...)
        return instance
    end
    return cls
end  

function __TRACKBACK__(msg) 
    print("----------------------------------------")
    print("LUA ERROR: " .. tostring( msg ))
    print(debug.traceback() )
    print("----------------------------------------")  
end    
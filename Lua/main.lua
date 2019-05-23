local _metaobj = {
	__index = function(this, key)
		if this._instance[key] ~= nil then
			return this._instance[key]
		end
		return this._prototype[key]
	end,
	__newindex = function(this, key, value)
		if this._instance[key] ~= nil then
			if(type(this._instance[key]) ~= type(value)) then
				error("attempt to assign "..type(value).." ("..value..") to "..type(this._instance[key]).." field", 2)
			end
			this._instance[key] = value
		else
			rawset(this, key, value)
		end
	end
}

local function _index (this, key)
	for _, v in ipairs(this._indices) do
		if v[key] then
			return v[key]
		end
	end
end

function istype(this, path)
	return this["_"..path]
end

local function _new(base, t)
	if type(t) == 'userdata' then
		t = {_instance=t}
		t._instance.instance = t
	else
		t = t or {}
		t._instance = _object(t)
	end
	t._prototype = base
	setmetatable(t,_metaobj)
	t:New()
	return t
end

local _metaclass = {
	__call = _new,
	__index = function(this, key)
		return this._prototype[key]
	end
}

local function _class(name, constructor)
	local C = {istype = istype}
	print("Loading class "..name)
	_G[name] = C
	C._type = name
	C["_"..name] = true
	setfenv(constructor or 3, setmetatable({_indices = {C, _G}},{__index = _index, __newindex = C}))
	return C
end

function base(m, name, constructor)
	local C = _class(name, constructor)
	setmetatable(C,{
		__call = _new
	})
	for k,v in pairs(m) do
		if not k:match("^__") and type(v) == 'function' then
			print ("Bound "..name..":"..k)
			C[k] = function (this, ...)
				return this._instance[k](this._instance, ...)
			end
		end
	end
	return constructor and constructor()
end

function class(m, name, constructor)
	local C = _class(name, constructor)
	C._prototype = m
	setmetatable(C,_metaclass)
	return constructor and constructor()
end

dofile "Lua/defs.lua"
dofile "Lua/content.lua"

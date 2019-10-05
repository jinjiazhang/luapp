module = "mongodb"

mongo_callbacks = mongo_callbacks or {}

function init( ... )
	setmetatable(_ENV, {__index = __index})
	_mongo = mongo.create_pool("mongodb://jinjiazh.com:27017", 5)
	_mongo.on_respond = on_respond
	copool.fork(init_schema)
end

function on_respond( token, ... )
	-- log_debug("mongo.on_respond", token, ...)
	local proc_func = mongo_callbacks[token]
	if proc_func then
		mongo_callbacks[token] = nil
		local status, errmsg = xpcall(proc_func, debug.traceback, ...)
		if not status then
			log_error("mongo.on_respond xpcall fail", errmsg)
		end
	end
end

function __index( env, key )
	if _G[key] then
		return _G[key]
	elseif type(_mongo[key]) == 'function' then
		env[key] = create_co_func(key, _mongo[key])
		return env[key]
	end
end

function create_co_func( name, pool_func )
	return function(...)
		local token = pool_func(...)
		if not token then
			log_error("mongo.co_func call fail", name, ...)
			return -1
		end

		local co = coroutine.running()
		mongo_callbacks[token] = function(...)
			local status, errmsg = coroutine.resume(co, ...)
			if not status then
				log_error("mongo.co_func resume fail", name, errmsg)
			end
		end
		return coroutine.yield("EXIT")
	end
end

function init_schema(  )
	local code, result = mongodb.mongo_command("admin", {["listDatabases"] = 1})
	if code ~= 0 then
		log_error("mongo.init_schema show tables fail", result)
		return
	end

	-- log_info("show dbs", app.tostring(result))

	local need_tabls = {
		["tb_global"] = {primarys = {"name"}},
		["tb_online"] = {primarys = {"openid"}},
		["tb_name"] = {primarys = {"name"}},
		["tb_account"] = {primarys = {"openid"}},
		["tb_role"] = {primarys = {"roleid"}},
		["tb_profile"] = {primarys = {"roleid"}},
		["tb_room"] = {primarys = {"roomid"}},
	}
end
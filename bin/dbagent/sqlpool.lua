-- mysql连接池
module = "sqlpool"

sqlpool_callbacks = sqlpool_callbacks or {}

function init( ... )
	setmetatable(_ENV, {__index = __index})
	mysql.parse("proto/db.proto")
	pool = mysql.create_pool()
	pool.on_respond = on_respond
	pool.connect("10.125.30.158", "jinjiazh", "10086", "game", 3306)
end

function on_respond( token, ret_code, ... )
	log_info("sqlpool.on_respond", token, ret_code, ...)
	local proc_func = sqlpool_callbacks[token]
	if proc_func then
		sqlpool_callbacks[token] = nil
		local status, errmsg = xpcall(proc_func, debug.traceback, ret_code, ...)
		if not status then
			log_error("sqlpool.on_respond xpcall fail", errmsg)
		end
	end
end

function __index( env, key )
	if _G[key] then
		return _G[key]
	elseif type(pool[key]) == 'function' then
		env[key] = create_co_func(key, pool[key])
		return env[key]
	end
end

function create_co_func( name, pool_func )
	return function(...)
		local token = pool_func(...)
		if not token then
			log_error("sqlpool.co_func call fail", name, ...)
			return -1
		end

		local co = coroutine.running()
		sqlpool_callbacks[token] = function(...)
			local status, errmsg = coroutine.resume(co, ...)
			if not status then
				log_error("sqlpool.co_func resume fail", name, errmsg)
			end
		end
		return coroutine.yield("EXIT")
	end
end
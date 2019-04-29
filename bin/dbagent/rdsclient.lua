-- redis连接实例
module = "rdsclient"

rdsclient_callbacks = rdsclient_callbacks or {}

function init( ... )
	setmetatable(env, {__index = __index})
	client = redis.connect("10.125.30.158", 6379)
	client.on_connect = on_connect
	client.on_disconnect = on_disconnect
	client.on_reply = on_reply
end

function on_connect( status, errmsg )
	log_info("rdsclient.on_connect", status, errmsg)
end

function on_disconnect( status, errmsg )
	log_info("rdsclient.on_disconnect", status, errmsg)
end

function on_reply( token, ret_code, ... )
	log_info("rdsclient.on_reply", token, ret_code, ...)
	local proc_func = rdsclient_callbacks[token]
	if proc_func then
		rdsclient_callbacks[token] = nil
		local status, errmsg = xpcall(proc_func, debug.traceback, ret_code, ...)
		if not status then
			log_error("rdsclient.on_reply xpcall fail", errmsg)
		end
	end
end

function __index( env, key )
	if _G[key] then
		return _G[key]
	elseif type(client[key]) == 'function' then
		env[key] = create_co_func(key, client[key])
		return env[key]
	end
end

function create_co_func( name, pool_func )
	return function(...)
		local token = pool_func(...)
		if not token then
			log_error("rdsclient.co_func call fail", name, ...)
			return false
		end

		local co = coroutine.running()
		rdsclient_callbacks[token] = function(...)
			local status, errmsg = coroutine.resume(co, ...)
			if not status then
				log_error("rdsclient.co_func resume fail", name, errmsg)
			end
		end
		return coroutine.yield("EXIT")
	end
end
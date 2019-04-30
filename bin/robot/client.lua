-- 网络客户端
module = "client"

client_callbacks = client_callbacks or {}

function init( ... )
	setmetatable(_ENV, {__index = __index})
	conn = net.connect(config.lobby_ip, config.lobby_port)
	conn.on_accept = on_accept
	conn.on_closed = on_closed
	conn.on_message = on_message
end

function __index( env, key )
	if _G[key] then
		return _G[key]
	elseif proto.belong(key) then
		env[key] = create_co_func(key)
		return env[key]
	end
end

function new_flowid(  )
	flowid = (flowid or 0) + 1
	return flowid
end

function create_co_func( proto )
	return function(...)
		local flowid = new_flowid()
		conn.call(proto, flowid, ...)

		local co = coroutine.running()
		client_callbacks[flowid] = function(...)
			local status, errmsg = coroutine.resume(co, ...)
			if not status then
				log_error("client.co_func resume fail", proto, errmsg)
			end
		end
		return coroutine.yield("EXIT")
	end
end

function on_accept( number, errno )
	log_info("client.on_accept", number, errno)
end

function on_closed( number, errno )
	log_info("client.on_closed", number, errno)
end

function on_message( number, proto, flowid, ... )
	log_info("client.on_message", number, proto, flowid, ...)
	local proc_func = client_callbacks[flowid]
	if proc_func then
		client_callbacks[flowid] = nil
		local status, errmsg = xpcall(proc_func, debug.traceback, ...)
		if not status then
			log_error("client.on_message xpcall fail", errmsg)
		end
		return
	end

	local proc_func = net[proto]
	if not proc_func then
		log_error("client.on_message proc_func not found", proto)
		return
	end

	proc_func(...)
end
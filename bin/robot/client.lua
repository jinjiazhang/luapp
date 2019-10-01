module = "client"

last_send_flowid = last_send_flowid or 0
last_recv_flowid = last_recv_flowid or 0
client_callbacks = client_callbacks or {}
client_callnames = client_callnames or {}

function init( ... )
	setmetatable(_ENV, {__index = __index})
	_client = net.connect("127.0.0.1", config.lobby_port)
	_client.on_accept = on_accept
	_client.on_closed = on_closed
	_client.on_message = on_message
end

function __index( env, key )
	if _G[key] then
		return _G[key]
	elseif proto.belong(key) then
		env[key] = create_co_func(key)
		return env[key]
	end
end

function create_co_func( proto )
	return function(...)
		local flowid = last_send_flowid + 1
		last_send_flowid = flowid
		_client.call(proto, ...)
		local co = coroutine.running()
		client_callnames[flowid] = proto
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

function on_message( number, msg_name, ... )
	log_info("client.on_message", number, msg_name, ...)
	if string.find(msg_name, "rsp$") then
		local flowid = last_recv_flowid + 1
		last_recv_flowid = flowid
		local proc_func = client_callbacks[flowid]
		local call_name = client_callnames[flowid]
		if not proc_func or not call_name then
			log_error("client.on_message callback not found", msg_name)
			for k, v in pairs(client_callnames) do
				log_error(k, v)
			end
			return
		end

		local proc_name = string.gsub(call_name, "req$", "rsp")
		if msg_name ~= proc_name then
			log_error("client.on_message call_name error", call_name, msg_name)
			return
		end

		client_callbacks[flowid] = nil
		client_callnames[flowid] = nil
		local status, errmsg = xpcall(proc_func, debug.traceback, ...)
		if not status then
			log_error("client.on_message xpcall fail", errmsg)
		end
		return
	end

	local proc_func = net[msg_name]
	if not proc_func then
		log_error("client.on_message proc_func not found", msg_name)
		return
	end

	proc_func(...)
end
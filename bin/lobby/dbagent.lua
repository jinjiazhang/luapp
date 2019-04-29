-- dbagent客户端
module = "dbagent"

function init( ... )
	setmetatable(env, {__index = __index})
	client = net.connect("127.0.0.1", 8086)
	client.on_accept = on_accept
	client.on_closed = on_closed
	client.on_message = on_message
end

function __index( env, key )
	if _G[key] then
		return _G[key]
	elseif proto.belong(key) then
		env[key] = function ( ... )
			client.call(key, ...)
		end
		return env[key]
	end
end

function on_accept( number, errno )
	log_info("dbagent.on_accept", number, errno)
end

function on_closed( number, errno )
	log_info("dbagent.on_closed", number, errno)
end

function on_message( number, proto, ... )
	log_info("dbagent.on_message", number, proto, ...)
	local proc_func = net[proto]
	if not proc_func then
		log_error("dbagent.on_message proc_func not found", proto)
		return
	end

	proc_func(...)
end
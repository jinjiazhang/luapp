module = "dbagent"

function init( ... )
	setmetatable(_ENV, {__index = __index})
	_client = net.connect(config.dbagent_ip, config.dbagent_port)
	_client.on_accept = on_accept
	_client.on_closed = on_closed
	_client.on_message = on_message
end

function __index( env, key )
	if _G[key] then
		return _G[key]
	elseif proto.belong(key) then
		env[key] = function ( ... )
			_client.call(key, ...)
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
	-- log_debug("dbagent.on_message", number, proto, ...)
	local proc_func = net[proto]
	if not proc_func then
		log_error("dbagent.on_message proc_func not found", proto)
		return
	end

	proc_func(...)
end
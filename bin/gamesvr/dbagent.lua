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
	elseif proto.exist(key) then
		env[key] = function ( ... )
			_client.call(key, ...)
		end
		return env[key]
	end
end

function on_accept( netid, errno )
	log_info("dbagent.on_accept", netid, errno)
end

function on_closed( netid, errno )
	log_info("dbagent.on_closed", netid, errno)
end

function on_message( netid, proto, ... )
	-- log_debug("dbagent.on_message", netid, proto, ...)
	local proc_func = net[proto]
	if not proc_func then
		log_error("dbagent.on_message proc_func not found", proto)
		return
	end

	proc_func(...)
end
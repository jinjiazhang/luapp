module = "server"

function init( ... )
	_server = net.listen(config.lobby_ip, config.lobby_port)
	_server.on_accept = on_accept
	_server.on_closed = on_closed
	_server.on_message = on_message
end

function on_accept( number, errno )
	log_info("server.on_accept", number, errno)
	if errno == 0 then
		ssmgr.on_start(number)
	end
end

function on_closed( number, errno )
	log_info("server.on_closed", number, errno)
	ssmgr.on_stop(number)
end

function on_message( number, proto, ... )
	-- log_debug("server.on_message", number, proto, ...)
	ssmgr.on_call(number, proto, ...)
end

function close_conn( number )
	log_info("server.close_conn", number)
	net.close(number)
	ssmgr.on_stop(number)
end
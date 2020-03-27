module = "server"

function init( ... )
	_gateway = gateway.connect(config.gateway_ip, config.gateway_port)
	_gateway.on_accept = on_accept
	_gateway.on_closed = on_closed
	_gateway.on_message = on_message
	
	_gateway.on_start = on_start
	_gateway.on_stop = on_stop
	_gateway.on_transmit = on_transmit
end

function on_accept( svrid, errno )
	log_info("gateway.on_accept", svrid, errno)
end

function on_closed( svrid, errno )
	log_info("gateway.on_closed", svrid, errno)
end

function on_message( svrid, proto, ... )
	log_debug("gateway.on_message", svrid, proto, ...)
end

function on_start( connid, data )
	log_info("gateway.on_start", connid, data)
	ssmgr.on_start(connid)
end

function on_stop( connid )
	log_info("gateway.on_stop", connid)
	ssmgr.on_stop(connid)
end

function on_transmit( connid, proto, ... )
	-- log_debug("gateway.connid", connid, proto, ...)
	ssmgr.on_call(connid, proto, ...)
end

function call_client( connid, proto, ...)
	_gateway.transmit(connid, proto, ...)
end

function close_conn( connid )
	log_info("gateway.close_conn", connid)
	_gateway.stop(connid)
	ssmgr.on_stop(connid)
end
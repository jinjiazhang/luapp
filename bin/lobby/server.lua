module = "server"

function init( ... )
	_gwclient = gateway.connect(config.gateway_ip, config.gateway_port)
	_gwclient.on_accept = on_accept
	_gwclient.on_closed = on_closed
	_gwclient.on_message = on_message
	
	_gwclient.on_start = on_start
	_gwclient.on_stop = on_stop
	_gwclient.on_transmit = on_transmit
end

function on_accept( svrid, errno )
	log_info("gwclient.on_accept", svrid, errno)
end

function on_closed( svrid, errno )
	log_info("gwclient.on_closed", svrid, errno)
end

function on_message( svrid, proto, ... )
	log_debug("gwclient.on_message", svrid, proto, ...)
end

function on_start( connid, data )
	log_info("gwclient.on_start", connid, data)
	ssmgr.on_start(connid)
end

function on_stop( connid )
	log_info("gwclient.on_stop", connid)
	ssmgr.on_stop(connid)
end

function on_transmit( connid, proto, ... )
	-- log_debug("gwclient.connid", connid, proto, ...)
	ssmgr.on_call(connid, proto, ...)
end

function call_client( connid, proto, ...)
	_gwclient.transmit(connid, proto, ...)
end

function close_conn( connid )
	log_info("gwclient.close_conn", connid)
	_gwclient.stop(connid)
	ssmgr.on_stop(connid)
end

function init( ... )
	_server = net.listen(config.dbagent_ip, config.dbagent_port)
	_server.on_accept = on_accept
	_server.on_closed = on_closed
	_server.on_message = on_message
end

function on_accept( netid, errno )
	log_info("server.on_accept", netid, errno)
	if errno == 0 then
		ssmgr.on_start(netid)
	end
end

function on_closed( netid, errno )
	log_info("server.on_closed", netid, errno)
	ssmgr.on_stop(netid)
end

function on_message( netid, proto, ... )
	-- log_debug("server.on_message", netid, proto, ...)
	ssmgr.on_call(netid, proto, ...)
end
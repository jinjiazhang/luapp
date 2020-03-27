svrid_list = svrid_list or {}

function init( ... )
	_server = gateway.listen(config.gateway_ip, config.gateway_port)
	_server.on_accept = on_accept_server
	_server.on_closed = on_closed_server
	_server.on_message = on_server_message

	_proxy = _server.open(config.proxy_ip, config.proxy_port)
	_proxy.on_accept = on_accept_client
	_proxy.on_closed = on_closed_client
	_proxy.on_message = on_client_message
end

function on_accept_server( svrid, errno )
	log_info("server.on_accept_server", svrid, errno)
	table.insert(svrid_list, svrid)
end

function on_closed_server( svrid, errno )
	log_info("server.on_closed_server", svrid, errno)
	for i = #svrid_list, 1, -1 do
		if svrid_list[i] == svrid then
			table.remove(svrid_list, i)
		end
	end
end

function on_server_message( svrid, proto, ... )
	log_debug("server.on_server_message", svrid, proto, ...)
end

function on_accept_client( connid, errno )
	log_info("proxy.on_accept_client", connid, errno)
	_server.start(connid, svrid_list[1], "proxy")
end

function on_closed_client( connid, errno )
	log_info("proxy.on_closed_client", connid, errno)
	_server.stop(connid, svrid_list[1])
end

function on_client_message( connid, proto, ... )
	log_debug("proxy.on_client_message", connid, proto, ...)
	
end
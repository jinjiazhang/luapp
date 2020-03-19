module = "airport"

function init( ... )
	setmetatable(_ENV, {__index = __index})
	_airport = route.connect(config.router_ip, config.router_port)
	_airport.on_accept = on_accept
	_airport.on_closed = on_closed
	_airport.on_message = on_message
	_airport.on_transmit = on_transmit
end

function __index( env, key )
	if _G[key] then
		return _G[key]
	else
		return _airport[key]
	end
end

function get_area_id( svrid )
	return (svrid >> 24) % 256
end

function get_zone_id( svrid )
	return (svrid >> 16) % 256
end

function get_func_id( svrid )
	return (svrid >> 8) % 256
end

function get_inst_id( svrid )
	return (svrid >> 0) % 256
end

function svrid_itos( svrid )
	return string.format("%d.%d.%d.%d",
		get_area_id( svrid ),
		get_zone_id( svrid ),
		get_func_id( svrid ),
		get_inst_id( svrid ))
end

function on_accept( svrid, errno )
	log_info("airport.on_accept", svrid_itos(svrid), errno)
end

function on_closed( svrid, errno )
	log_info("airport.on_closed", svrid_itos(svrid), errno)
end

function on_message( svrid, proto, ... )
	-- log_debug("airport.on_message", svrid_itos(svrid), proto, ...)
	local proc_func = net[proto]
	if not proc_func then
		log_error("airport.on_message proc_func not found", proto)
		return
	end

	proc_func(svrid, ...)
end

function on_transmit( svrid, roleid, proto, ... )
	log_error("airport.on_transmit", roleid, proto, ...)
end

function call_lobby( lobbyid, ... )
	if get_func_id(lobbyid) ~= service.LOBBY then
		log_error("airport.call_lobby funcid error", lobbyid)
		return
	end
	_airport.call_target(lobbyid, ...)
end

function call_gamesvr( gsvrid, ... )
	if get_func_id(gsvrid) ~= service.GAMESVR then
		log_error("airport.call_gamesvr funcid error", gsvrid)
		return
	end
	_airport.call_target(gsvrid, ...)
end

function call_listsvr( lsvrid, ... )
	if get_func_id(lsvrid) ~= service.LISTSVR then
		log_error("airport.call_listsvr funcid error", lsvrid)
		return
	end
	_airport.call_target(lsvrid, ...)
end

function call_listsvr_hash( mode, ... )
	_airport.call_random(service.LISTSVR, ...)
end

function call_listsvr_all( ... )
	_airport.call_group(service.LISTSVR, ...)
end
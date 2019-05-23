module = "airport"

function init( ... )
	setmetatable(_ENV, {__index = __index})
	port = route.connect(config.router_ip, config.router_port)
	port.on_accept = on_accept
	port.on_closed = on_closed
	port.on_message = on_message
	port.on_transmit = on_transmit
end

function __index( env, key )
	if _G[key] then
		return _G[key]
	elseif port[key] then
		return port[key]
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
	-- log_info("airport.on_message", svrid_itos(svrid), proto, ...)
	local proc_func = net[proto]
	if not proc_func then
		log_error("airport.on_message proc_func not found", proto)
		return
	end

	proc_func(svrid, ...)
end

function on_transmit( roleid, proto, ... )
	log_info("airport.on_transmit", roleid, proto, ...)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		return
	end

	local proc_func = net[proto]
	if proc_func then
		proc_func(ss, ...)
		return
	end

	ss[proto](...)
end

function call_lobby( lobbyid, ... )
	if get_func_id(lobbyid) ~= service.LOBBY then
		log_error("airport.call_lobby funcid error", lobbyid)
		return
	end
	airport.call_target(lobbyid, ...)
end

function call_roomsvr( rsvrid, ... )
	if get_func_id(rsvrid) ~= service.ROOMSVR then
		log_error("airport.call_roomsvr funcid error", rsvrid)
		return
	end
	airport.call_target(rsvrid, ...)
end

function call_listsvr( lsvrid, ... )
	if get_func_id(lsvrid) ~= service.LISTSVR then
		log_error("airport.call_listsvr funcid error", lsvrid)
		return
	end
	airport.call_target(lsvrid, ...)
end

function call_listsvr_mode( mode, ... )
	port.call_random(service.LISTSVR, ...)
end

function call_listsvr_all( ... )
	port.call_group(service.LISTSVR, ...)
end
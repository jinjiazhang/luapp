module = "remote"

function init( ... )
	setmetatable(_ENV, {__index = __index})
	_rtclient = route.connect(config.router_ip, config.router_port)
	_rtclient.on_accept = on_accept
	_rtclient.on_closed = on_closed
	_rtclient.on_message = on_message
	_rtclient.on_transmit = on_transmit
end

function __index( env, key )
	if _G[key] then
		return _G[key]
	else
		return _rtclient[key]
	end
end

function on_accept( svrid, errno )
	log_info("remote.on_accept", svrid_itos(svrid), errno)
end

function on_closed( svrid, errno )
	log_info("remote.on_closed", svrid_itos(svrid), errno)
end

function on_message( svrid, proto, ... )
	-- log_debug("remote.on_message", svrid_itos(svrid), proto, ...)
	local proc_func = net[proto]
	if not proc_func then
		log_error("remote.on_message proc_func not found", proto)
		return
	end

	proc_func(svrid, ...)
end

function on_transmit( svrid, roleid, proto, ... )
	log_debug("remote.on_transmit", roleid, proto, ...)
	if get_func_id(app.svrid()) == service.LOBBY then
		local ss = ssmgr.find_by_roleid(roleid)
		if not ss then
			log_warn("remote.on_transmit ss not exist", roleid, proto)
			return
		end

		local proc_func = net[proto]
		if proc_func then
			proc_func(ss, ...)
			return
		end

		ss[proto](...)
	else
		local proc_func = net[proto]
		if not proc_func then
			log_error("remote.on_transmit proc_func not found", proto)
			return
		end
	
		proc_func(roleid, ...)
	end
end

function call_client( roleid, ... )
	if get_func_id(app.svrid()) == service.LOBBY then
		log_error("remote.call_client not used in lobby")
		return
	end
	_rtclient.call_transmit(service.LOBBY, roleid, ...)
end

function call_lobby( lobbyid, ... )
	if get_func_id(lobbyid) ~= service.LOBBY then
		log_error("remote.call_lobby funcid error", lobbyid)
		return
	end
	_rtclient.call_target(lobbyid, ...)
end

function call_gamesvr( gsvrid, ... )
	if get_func_id(gsvrid) ~= service.GAMESVR then
		log_error("remote.call_gamesvr funcid error", gsvrid)
		return
	end
	_rtclient.call_target(gsvrid, ...)
end

function call_roomsvr( lsvrid, ... )
	if get_func_id(lsvrid) ~= service.ROOMSVR then
		log_error("remote.call_roomsvr funcid error", lsvrid)
		return
	end
	_rtclient.call_target(lsvrid, ...)
end

function call_roomsvr_hash( mode, ... )
	_rtclient.call_random(service.ROOMSVR, ...)
end

function call_roomsvr_all( ... )
	_rtclient.call_group(service.ROOMSVR, ...)
end
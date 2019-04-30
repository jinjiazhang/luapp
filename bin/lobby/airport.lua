-- 消息路由客户端
module = "airport"

function init( ... )
	setmetatable(_ENV, {__index = __index})
	port = route.connect("127.0.0.1", 8087)
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
	log_info("airport.on_message", svrid_itos(svrid), proto, ...)
	local proc_func = net[proto]
	if not proc_func then
		log_error("airport.on_message proc_func not found", proto)
		return
	end

	proc_func(svrid, ...)
end

function on_transmit( roleid, proto, ... )
	log_info("airport.on_transmit", roleid, proto, ...)
end
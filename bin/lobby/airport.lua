-- 消息路由客户端

function init( ... )
	airport = route.connect("127.0.0.1", 8087)
	airport.on_accept = on_accept
	airport.on_closed = on_closed
	airport.on_message = on_message
	airport.on_transmit = on_transmit
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

function on_accept( svrid, error )
	log_info("airport.on_accept", svrid_itos(svrid), error)
end

function on_closed( svrid, error )
	log_info("airport.on_closed", svrid_itos(svrid), error)
end

function on_message( svrid, proto, ... )
	log_info("airport.on_message", svrid_itos(svrid), proto, ...)
end

function on_transmit( roleid, proto, ... )
	log_error("airport.on_transmit", roleid, proto, ...)
end
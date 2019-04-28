-- 消息路由服务器

function init( ... )
	router = route.listen("127.0.0.1", 8087)
	router.on_accept = on_accept
	router.on_closed = on_closed
	router.on_message = on_message
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
	log_info("router.on_accept", svrid_itos(svrid), errno)

	local func_id = get_func_id(svrid)
	router.set_group(svrid, func_id)
end

function on_closed( svrid, errno )
	log_info("router.on_closed", svrid_itos(svrid), errno)
	
	router.set_group(svrid, 0)
end

function on_message( svrid, proto, ... )
	log_info("router.on_message", svrid_itos(svrid), proto, ...)
end
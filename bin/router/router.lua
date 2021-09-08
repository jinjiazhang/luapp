
function init( ... )
	_router = route.listen(config.router_ip, config.router_port)
	_router.on_accept = on_accept
	_router.on_closed = on_closed
	_router.on_message = on_message
end

function on_accept( svrid, errno )
	log_info("router.on_accept", svrid_itos(svrid), errno)

	local func_id = get_func_id(svrid)
	_router.set_group(svrid, func_id)
end

function on_closed( svrid, errno )
	log_info("router.on_closed", svrid_itos(svrid), errno)
	
	_router.set_group(svrid, 0)
end

function on_message( svrid, proto, ... )
	log_debug("router.on_message", svrid_itos(svrid), proto, ...)
end
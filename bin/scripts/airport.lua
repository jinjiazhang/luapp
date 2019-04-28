-- 消息路由示例

local airport = nil
function init( ... )
	airport = route.connect("127.0.0.1", 8087)
	airport.on_accept = on_accept
	airport.on_closed = on_closed
	airport.on_message = on_message
	airport.on_transmit = on_transmit
end

function on_accept( svrid, errno )
	log_info("airport.on_accept", svrid, errno)
	airport.reg_role(1001)
	airport.call_client(1001, "on_login_req", "jinjiazh", "10086")
end

function on_closed( svrid, errno )
	log_info("airport.on_closed", svrid, errno)
end

function on_message( svrid, proto, ... )
	log_info("airport.on_message", proto, ...)
end

function on_transmit( roleid, proto, ... )
	log_info("airport.on_transmit", roleid, proto, ...)
end
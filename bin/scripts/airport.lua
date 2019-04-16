-- 消息路由示例

local airport = nil
function init( ... )
	airport = route.connect("127.0.0.1", 8087)
	airport.on_accept = on_accept
	airport.on_closed = on_closed
	airport.on_message = on_message
	airport.on_transmit = on_transmit
end

function on_accept( svrid, error )
	log_info("airport.on_accept", svrid, error)
	airport.reg_role(1001)
	airport.call_client(1001, "LoginReq", "jinjiazh", "10086")
end

function on_closed( svrid, error )
	log_info("airport.on_closed", svrid, error)
end

function on_message( svrid, proto, ... )
	log_info("airport.on_message", proto, ...)
end

function on_transmit( roleid, proto, ... )
	log_info("airport.on_transmit", roleid, proto, ...)
end
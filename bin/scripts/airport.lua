-- 消息路由示例

local airport = nil
function init( ... )
	airport = route.connect("127.0.0.1", 8087)
	airport.on_accept = on_accept
	airport.on_closed = on_closed
	airport.on_message = on_message
end

function on_accept( svrid, error )
	log_info("airport.on_accept", svrid, error)
end

function on_closed( svrid, error )
	log_info("airport.on_closed", svrid, error)
end

function on_message( svrid, proto, ... )
	log_info("airport.on_message", proto, ...)
end
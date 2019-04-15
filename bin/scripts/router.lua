-- 消息路由示例

local router = nil
function init( ... )
	router = route.listen(101, "127.0.0.1", 8087)
	router.on_accept = on_accept
	router.on_closed = on_closed
	router.on_message = on_message
end

function on_accept( svrid, error )
	log_info("router.on_accept", svrid, error)
end

function on_closed( svrid, error )
	log_info("router.on_closed", svrid, error)
end

function on_message( svrid, proto, ... )
	log_info("router.on_message", proto, ...)
end
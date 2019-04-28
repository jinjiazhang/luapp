-- 监听端口服务

function init( ... )
	server = net.listen("127.0.0.1", 8088)
	server.on_accept = on_accept
	server.on_closed = on_closed
	server.on_message = on_message
end

function on_accept( number, error )
	log_info("server.on_accept", number, error)
end

function on_closed( number, error )
	log_info("server.on_closed", number, error)
end

function on_message( number, proto, ... )
	log_info("server.on_message", number, proto, ...)
end
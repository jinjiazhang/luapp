-- 监听端口服务

function init( ... )
	server = net.listen("127.0.0.1", 8086)
	server.on_accept = on_accept
	server.on_closed = on_closed
	server.on_message = on_message
end

function on_accept( number, errno )
	log_info("listen.on_accept", number, errno)
end

function on_closed( number, errno )
	log_info("listen.on_closed", number, errno)
end

function on_message( number, proto, ... )
	log_info("listen.on_message", number, proto, ...)
end
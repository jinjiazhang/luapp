-- 网络客户端

function init( ... )
	client = net.connect("127.0.0.1", 8088)
	client.on_accept = on_accept
	client.on_closed = on_closed
	client.on_message = on_message
end

function on_accept( number, error )
	log_info("client.on_accept", number, error)
end

function on_closed( number, error )
	log_info("client.on_closed", number, error)
end

function on_message( number, proto, ... )
	log_info("client.on_message", number, proto, ...)
end
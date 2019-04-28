-- dbagent客户端

function init( ... )
	client = net.connect("127.0.0.1", 8086)
	client.on_accept = on_accept
	client.on_closed = on_closed
	client.on_message = on_message
end

function on_accept( number, errno )
	log_info("dbagent.on_accept", number, errno)
end

function on_closed( number, errno )
	log_info("dbagent.on_closed", number, errno)
end

function on_message( number, proto, ... )
	log_info("dbagent.on_message", number, proto, ...)
end
-- 监听端口服务

function init( ... )
	server = net.listen("127.0.0.1", 8088)
	server.on_accept = on_accept
	server.on_closed = on_closed
	server.on_message = on_message
end

function on_accept( number, errno )
	log_info("server.on_accept", number, errno)
	if errno == 0 then
		ssmgr.on_start(number)
	end
end

function on_closed( number, errno )
	log_info("server.on_closed", number, errno)
	ssmgr.on_stop(number)
end

function on_message( number, proto, ... )
	log_info("server.on_message", number, proto, ...)
	ssmgr.on_call(number, proto, ...)
end
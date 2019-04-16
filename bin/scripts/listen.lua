-- 监听端口示例

server = nil -- 监听实例
function init( ... )
	server = net.listen("127.0.0.1", 8086)
	server.on_accept = on_accept
	server.on_closed = on_closed
	server.on_message = on_message
end

function on_accept( number, error )
	log_info("listen.on_accept", number, error)
end

function on_closed( number, error )
	log_info("listen.on_closed", number, error)
end

function on_message( number, proto, ... )
	log_info("listen.on_message", number, proto, ...)
	net.call(number, "LoginRsp", LoginResult.SUCCESS)
end
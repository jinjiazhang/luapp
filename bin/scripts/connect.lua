-- 连接端口示例

client = nil -- 监听实例
function init( ... )
	client = net.connect("127.0.0.1", 8086)
	client.on_accept = on_accept
	client.on_closed = on_closed
	client.on_message = on_message
end

function on_accept( number, error )
	log_info("connect.on_accept", number, error)
	net.call(number, "LoginReq", "jinjiazh", "10086")
end

function on_closed( number, error )
	log_info("connect.on_closed", number, error)
end

function on_message( number, proto, ... )
	log_info("connect.on_message", number, proto, ...)
	client.close()
end
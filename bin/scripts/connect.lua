-- 连接端口示例

client = nil -- 监听实例
function init( ... )
	client = net.connect("127.0.0.1", 8086)
	client.on_accept = on_accept
	client.on_closed = on_closed
	client.on_message = on_message
end

function on_accept( number, errno )
	log_info("connect.on_accept", number, errno)
	net.call(number, "cs_login_req", "jinjiazh", "10086")
end

function on_closed( number, errno )
	log_info("connect.on_closed", number, errno)
end

function on_message( number, proto, ... )
	log_info("connect.on_message", number, proto, ...)
	client.close()
end
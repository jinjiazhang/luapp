-- 连接端口示例

client = nil -- 监听实例
function init( ... )
	client = net.connect("127.0.0.1", 8087)
	client.on_accept = on_accept
	client.on_closed = on_closed
	client.on_package = on_package
end

function on_accept( number, error )
	log_info("connect.on_accept", number, error)
	local data = "hello, I am client-"..number
	net.send(number, data)
end

function on_closed( number, error )
	log_info("connect.on_closed", number, error)
end

function on_package( number, data )
	log_info("connect.on_package", number, data)
	client.close()
end
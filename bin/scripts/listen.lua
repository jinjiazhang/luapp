-- 监听端口示例

server = nil -- 监听实例
function init( ... )
	server = net.listen("127.0.0.1", 8087)
	server.on_accept = on_accept
	server.on_closed = on_closed
	server.on_package = on_package
end

function on_accept( number, error )
	log_info("listen.on_accept", number, error)
end

function on_closed( number, error )
	log_info("listen.on_closed", number, error)
end

function on_package( number, data )
	log_info("listen.on_package", number, data)
	local data = "hello, I am server"
	net.send(number, data)
end
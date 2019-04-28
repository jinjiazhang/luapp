-- 网络客户端
flowid = flowid or 1

function init( ... )
	client = net.connect("127.0.0.1", 8088)
	client.on_accept = on_accept
	client.on_closed = on_closed
	client.on_message = on_message
end

function on_accept( number, errno )
	log_info("client.on_accept", number, errno)
end

function on_closed( number, errno )
	log_info("client.on_closed", number, errno)
end

function on_message( number, proto, ... )
	log_info("client.on_message", number, proto, ...)
end

function new_flowid(  )
	flowid = flowid + 1
	return flowid
end

function request_login( openid, token )
	client.call("cs_login_req", flowid, openid, token)
end
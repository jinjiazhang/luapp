client = nil -- 连接实例
function init( ... )
	client = redis.connect("10.125.30.158", 6379)
	client.on_connect = on_connect
	client.on_disconnect = on_disconnect
	client.on_reply = on_reply

	client.command("get", "counter")
end

function on_connect( status )
	log_debug("rdsclient.on_connect", status)
end

function on_disconnect( status )
	log_info("rdsclient.on_disconnect", status)
end

function on_reply( token, data )
	log_info("rdsclient.on_reply", token, data)
end
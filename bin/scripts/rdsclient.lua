client = nil -- 连接实例
function init( ... )
	client = redis.connect("10.125.30.158", 6379)
	client.on_connect = on_connect
	client.on_disconnect = on_disconnect
	client.on_reply = on_reply

	client.rds_command("get", "counter")
end

function on_connect( status, errmsg )
	log_debug("rdsclient.on_connect", status, errmsg)
end

function on_disconnect( status, errmsg )
	log_info("rdsclient.on_disconnect", status, errmsg)
end

function on_reply( token, status, data )
	log_info("rdsclient.on_reply", token, status, data)
end
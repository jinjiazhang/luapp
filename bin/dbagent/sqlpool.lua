-- mysql连接池

function init( ... )
	mysql.parse("protos/db.proto")
	pool = mysql.create_pool()
	pool.on_respond = on_respond
	pool.connect("10.125.30.158", "jinjiazh", "10086", "game", 3306)
end

function on_respond( token, ret_code, ... )
	log_info("sqlpool.on_respond", token, ret_code, ...)
end
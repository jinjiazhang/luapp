pool = nil -- 连接实例
function init( ... )
	mysql.parse("db.proto")
	pool = mysql.create_pool()
	pool.connect("10.125.30.158", "jinjiazh", "10086", "game", 3306)
	pool.on_respond = on_respond
	pool.sql_select("user", "id = 'test004'")
end

function on_respond( token, ret_code, ... )
	log_debug("sqlpool.on_respond", token, ret_code, ...)
end
pool = nil -- 连接实例
function init( ... )
	mysql.parse("protos/db.proto")
	pool = mysql.create_pool()
	pool.connect("10.125.30.158", "jinjiazh", "10086", "game", 3306)
	pool.connect("10.125.30.158", "jinjiazh", "10086", "game", 3306)
	pool.connect("10.125.30.158", "jinjiazh", "10086", "game", 3306)
	pool.connect("10.125.30.158", "jinjiazh", "10086", "game", 3306)
	pool.connect("10.125.30.158", "jinjiazh", "10086", "game", 3306)
	pool.on_respond = on_respond

	-- pool.sql_create("tb_global")
	-- pool.sql_create("tb_account")
	-- pool.sql_create("tb_role")
	-- pool.sql_create("tb_profile")
	-- pool.sql_create("tb_name")
	-- pool.sql_create("tb_online")
	-- pool.sql_create("tb_room")
	pool.sql_execute("show tables")
end

function on_respond( token, ret_code, ... )
	log_debug("sqlpool.on_respond", token, ret_code, ...)
	for index, result in ipairs({...}) do
		log_debug("sqlpool.on_respond result:", index, proto.to_json(result))
	end
end
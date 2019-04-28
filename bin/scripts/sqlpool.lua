pool = nil -- 连接实例
function init( ... )
	mysql.parse("proto/db.proto")
	pool = mysql.create_pool()
	pool.connect("10.125.30.158", "jinjiazh", "10086", "game", 3306)
	-- pool.connect("10.125.30.158", "jinjiazh", "10086", "game", 3306)
	-- pool.connect("10.125.30.158", "jinjiazh", "10086", "game", 3306)
	-- pool.connect("10.125.30.158", "jinjiazh", "10086", "game", 3306)
	-- pool.connect("10.125.30.158", "jinjiazh", "10086", "game", 3306)
	pool.on_respond = on_respond

	-- pool.sql_create("tb_global")
	-- pool.sql_create("tb_account")
	-- pool.sql_create("tb_role")
	-- pool.sql_create("tb_profile")
	-- pool.sql_create("tb_name")
	-- pool.sql_create("tb_online")
	-- pool.sql_create("tb_room")

	pool.sql_execute("show tables")
	local data = {openid = "abc", roleid = 10001, svrid = 124, online = os.time()}
	pool.sql_insert("tb_online", data)
	pool.sql_select("tb_online", 'openid = "abc"')

	data.svrid = 137
	data.online = 0
	pool.sql_update("tb_online", data, 'openid = "abc"')
	pool.sql_select("tb_online", 'openid = "abc"')
	pool.sql_delete("tb_online", 'openid = "abc"')
end

function on_respond( token, ret_code, ... )
	log_debug("sqlpool.on_respond", token, ret_code, ...)
	for index, result in ipairs({...}) do
		log_debug("sqlpool.on_respond result:", index, proto.to_json(result))
	end
end
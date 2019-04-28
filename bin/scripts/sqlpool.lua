pool = nil -- 连接实例
function init( ... )
	mysql.parse("protos/db.proto")
	pool = mysql.create_pool()
	pool.connect("10.125.30.158", "jinjiazh", "10086", "game", 3306)
	pool.on_respond = on_respond

	local player = {
		name = "jinjiazh",
		id = 10001,
		email = "jinjiazh@qq.com",
		phones = {
			{number = "1818864xxxx", type = 1},
			{number = "1868200xxxx", type = 2},
		},
		subjects = {
			[101] = "Chinese",
			[102] = "English",
			[103] = "Maths",
		}
	}

	-- pool.sql_create("Person")
	pool.sql_execute("update Person set id = (@old := id) + 1;")
	-- pool.sql_insert("Person", player)
	-- pool.sql_select("Person", "id = 10001")
	-- 
	-- player.email = "jinjiazh@163.com"
	-- pool.sql_update("Person", player, "id = 10001")
	-- pool.sql_select("Person", "id = 10001")
end

function on_respond( token, ret_code, ... )
	log_debug("sqlpool.on_respond", token, ret_code, ...)
	for index, result in ipairs({...}) do
		log_debug("sqlpool.on_respond result:", index, proto.to_json(result))
	end
end
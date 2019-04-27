pool = nil -- 连接实例
function init( ... )
	mysql.parse("protos/db.proto")
	pool = mysql.create_pool()
	pool.connect("10.125.30.158", "jinjiazh", "10086", "game", 3306)
	pool.on_respond = on_respond
	
	pool.sql_create("Person")

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

	pool.sql_insert("Person", player)

	player.email = "jinjiazh@1163.com"
	pool.sql_update("Person", player, "id = 10001")
	pool.sql_select("Person", "id = 10001")
end

function on_respond( token, ret_code, ... )
	log_debug("sqlpool.on_respond", token, ret_code, ...)
	for index, result in ipairs({...}) do
		if type(result) == "table" then
			for k,v in pairs(result) do
				log_debug("sqlpool.on_respond result:", index, k, v)
			end
		end
	end
end
-- 机器人脚本

my_account = nil
my_role = nil
my_room = nil

function run( ... )
	local openid = string.format("openid_%d", app.time())
	run_test(login_flow, openid, "ABCDEFG")
end

function run_test( f, ... )
	local co = coroutine.create(f)
	local status, errmsg = coroutine.resume(co, ...)
	if not status then
		log_error("run_test fail", errmsg)
	end
end

function check_result( proto, result )
	if result ~= errno.SUCCESS then
		error(string.format("proto: %s, result: %d", proto, result), 2)
	end
end

function login_flow( openid, token )
	local result, account = client.cs_login_req(openid, token)
	log_info("cs_login_req", result, app.tostring(account))
	check_result("cs_login_req", result)
	my_account = account

	if account.roleid == 0 then
		local name = string.format("name_%d", app.time())
		local result, role = client.cs_create_role_req(name)
		log_info("cs_create_role_req", result, app.tostring(role))
		check_result("cs_create_role_req", result)
		my_role = role
	else
		local result, role = client.cs_select_role_req(account.roleid)
		log_info("cs_select_role_req", result, app.tostring(role))
		check_result("cs_select_role_req", result)
		my_role = role
	end

	local result, room_list = client.cs_fetch_room_req(game_mode.TEXAS)
	log_info("cs_fetch_room_req", result, app.tostring(room_list))
	check_result("cs_fetch_room_req", result)

	if #room_list == 0 then
		local option = proto.create("game_opt")
		option.texas = proto.create("texas_opt")
		option.texas.small_blind = 5
		option.texas.big_blind = 10
		local room_name = string.format("room_%d", app.time())
		local result, room = client.cs_create_room_req(room_name, game_mode.TEXAS, option)
		log_info("cs_create_room_req", result, app.tostring(room))
		check_result("cs_create_room_req", result)
		my_room = room

		-- local result = client.cs_leave_room_req(room.roomid)
		-- log_info("cs_leave_room_req", result)
		-- check_result("cs_leave_room_req", result)

		-- local result = client.cs_dismiss_room_req(room.roomid)
		-- log_info("cs_dismiss_room_req", result)
		-- check_result("cs_dismiss_room_req", result)
	else
		local info = room_list[1]
		local result, room = client.cs_enter_room_req(info.roomid, info.cipher)
		log_info("cs_enter_room_req", result, app.tostring(room))
		check_result("cs_enter_room_req", result)
		my_room = room

		local result = client.cs_room_chat_req(room.roomid, "I am "..my_role.name)
		log_info("cs_room_chat_req", result)
		check_result("cs_room_chat_req", result)
	end
end
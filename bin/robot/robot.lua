
function run( ... )
	local openid = string.format("openid_%d", app.time())
	run_test(texas_flow, openid, "ABCDEFG")
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

	if account.roleid == 0 then
		local name = string.format("name_%d", app.time())
		local result, role = client.cs_create_role_req(name)
		log_info("cs_create_role_req", result, app.tostring(role))
		check_result("cs_create_role_req", result)
		return role
	else
		local result, role = client.cs_select_role_req(account.roleid)
		log_info("cs_select_role_req", result, app.tostring(role))
		check_result("cs_select_role_req", result)
		return role
	end
	return nil
end

function create_room_flow( role )
	local option = proto.create("game_opt")
	option.texas = proto.create("texas_opt")
	option.texas.small_blind = 5
	option.texas.big_blind = 10
	local room_name = string.format("room_%d", app.time())
	local result, room = client.cs_create_room_req(room_name, game_mode.TEXAS, option)
	log_info("cs_create_room_req", result, app.tostring(room))
	check_result("cs_create_room_req", result)

	local game = room.game.texas
	local result, player = client.cs_texas_sitdown_req(1)
	log_info("cs_texas_sitdown_req", result)
	check_result("cs_texas_sitdown_req", result)
	table.insert(game.players, player)

	notify.wait_for_notify("texas_sitdown")

	log_info("send cs_texas_start_req")
	local result = client.cs_texas_start_req()
	log_info("cs_texas_start_req", result)
	check_result("cs_texas_start_req", result)
	return room
end

function enter_room_flow( role, roomid, roomkey )
	local result, room = client.cs_enter_room_req(roomid, roomkey)
	log_info("cs_enter_room_req", result, app.tostring(room))
	check_result("cs_enter_room_req", result)

	local result = client.cs_texas_chat_req("I_am_"..role.name)
	log_info("cs_texas_chat_req", result)
	check_result("cs_texas_chat_req", result)

	local game = room.game.texas
	local seatid = #game.players + 1
	local result, player = client.cs_texas_sitdown_req(seatid)
	log_info("cs_texas_sitdown_req", result)
	check_result("cs_texas_sitdown_req", result)
	table.insert(game.players, player)

	if not game.current then
		notify.wait_for_notify("texas_start")
	end
	return room
end

function texas_flow( openid, token )
	local role = login_flow(openid, token)
	local result, room_list = client.cs_fetch_room_req(game_mode.TEXAS)
	log_info("cs_fetch_room_req", result, app.tostring(room_list))
	check_result("cs_fetch_room_req", result)

	if #room_list == 0 then
		create_room_flow(role)
	else
		local info = room_list[1]
		enter_room_flow(role, info.roleid, info.roomkey)
	end

	local roomid, hand = notify.wait_for_notify("texas_hand")
	log_info("texas_hand", roomid, app.tostring(hand))

	local roomid, hand_idx, cards = notify.wait_for_notify("texas_deal")
	log_info("texas_deal", roomid, hand_idx, app.tostring(cards))

	log_info("test texas_flow finish!")
end
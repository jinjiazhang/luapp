module = "texas"
dealer = import("roomsvr/dealer.lua")

MIN_PLAYER_NUM = 2
MAX_PLAYER_NUM = 9

function on_create_room( room )
	-- log_info("on_create_room", room.roomid)
	local game = proto.create("texas_detail")
	game.roomid = room.roomid
	game.seat_table = {} -- seatid -> texas_player
	game.player_table = {} -- roleid -> texas_player
	game.button = 0
	game.hands = {}
	game.current = nil

	game.option = room.option.texas
	game.broadcast = room.broadcast
	room.game.texas = game
end

function on_enter_room( room, roleid )
	-- log_info("on_enter_room", room.roomid, roleid)
end

function on_leave_room( room, roleid )
	-- log_info("on_leave_room", room.roomid, roleid)
end

function on_tick_room( room )
	-- log_debug("on_tick_room", room.roomid)
	if room.status ~= room_status.PLAYING then
		return
	end

	local game = room.game.texas
	dealer.tick_game(game)
end

function net.cs_texas_chat_req( roleid, content )
	local room = roommgr.find_by_roleid(roleid)
	if not room then
		airport.call_client(roleid, "cs_texas_chat_rsp", errno.DATA_ERROR)
		return
	end

	if not room.get_viewer(roleid) then
		airport.call_client(roleid, "cs_texas_chat_rsp", errno.DATA_ERROR)
		return
	end

	room.broadcast(0,"cs_texas_chat_ntf", room.roomid, roleid, content)
	airport.call_client(roleid, "cs_texas_chat_rsp", errno.SUCCESS)
end

function net.cs_texas_sitdown_req( roleid, seatid )
	local room = roommgr.find_by_roleid(roleid)
	if not room then
		airport.call_client(roleid, "cs_texas_sitdown_rsp", errno.DATA_ERROR)
		return
	end

	local role = room.get_viewer(roleid)
	if not role then
		airport.call_client(roleid, "cs_texas_sitdown_rsp", errno.DATA_ERROR)
		return
	end

	if seatid <= 0 or seatid > MAX_PLAYER_NUM then
		airport.call_client(roleid, "cs_texas_sitdown_rsp", errno.PARAM_ERROR)
		return
	end

	local game = room.game.texas
	if game.player_table[roleid] then
		airport.call_client(roleid, "cs_texas_sitdown_rsp", errno.TEXAS_HAS_SEAT)
		return
	end

	if game.seat_table[seatid] then
		airport.call_client(roleid, "cs_texas_sitdown_rsp", errno.TEXAS_SEAT_USED)
		return
	end

	local player = {
		roleid = role.roleid,
		openid = role.openid,
		name = role.name,
		icon = role.icon,
		gender = role.gender,
		seatid = seatid,
		chips = 0,
		online = true,
		playing = true,
	}

	table.insert(game.players, player)
	game.player_table[roleid] = player
	game.seat_table[seatid] = player

	room.broadcast(roleid, "cs_texas_sitdown_ntf", room.roomid, player)
	airport.call_client(roleid, "cs_texas_sitdown_rsp", errno.SUCCESS, player)
end

function net.cs_texas_standup_req( roleid )
	local room = roommgr.find_by_roleid(roleid)
	if not room then
		airport.call_client(roleid, "cs_texas_standup_rsp", errno.DATA_ERROR)
		return
	end

	local game = room.game.texas
	local player = game.player_table[roleid]
	if not player then
		airport.call_client(roleid, "cs_texas_standup_rsp", errno.DATA_ERROR)
		return
	end

	for index, player in ipairs(game.players) do
		if roleid == player.roleid then
			table.remove(game.players, index)
			break
		end
	end

	local seatid = player.seatid
	game.seat_table[seatid] = nil
	game.player_table[roleid] = nil

	room.broadcast(roleid, "cs_texas_standup_ntf", room.roomid, seatid)
	airport.call_client(roleid, "cs_texas_standup_rsp", errno.SUCCESS)
end

function net.cs_texas_start_req( roleid )
	local room = roommgr.find_by_roleid(roleid)
	if not room then
		airport.call_client(roleid, "cs_texas_start_rsp", errno.DATA_ERROR)
		return
	end

	local game = room.game.texas
	local player = game.player_table[roleid]
	if not player then
		airport.call_client(roleid, "cs_texas_start_rsp", errno.DATA_ERROR)
		return
	end

	if roleid ~= room.master then
		airport.call_client(roleid, "cs_texas_start_rsp", errno.PRIVILEGE)
		return
	end

	if room.status ~= room_status.WAITING then
		airport.call_client(roleid, "cs_texas_start_rsp", errno.DATA_ERROR)
		return
	end

	if #game.players < MIN_PLAYER_NUM then
		airport.call_client(roleid, "cs_texas_start_rsp", errno.TEXAS_PLAYER_NUM)
		return
	end

	game.button = player.seatid
	room.status = room_status.PLAYING
	room.start_time = app.time()
	room.broadcast(roleid, "cs_texas_start_ntf", room.roomid)
	airport.call_client(roleid, "cs_texas_start_rsp", errno.SUCCESS)
end

function net.cs_texas_action_req( roleid, hand_idx, round_idx, act_type, act_chips )
	local room = roommgr.find_by_roleid(roleid)
	if not room then
		airport.call_client(roleid, "cs_texas_action_rsp", errno.DATA_ERROR)
		return
	end

	if room.status ~= room_status.PLAYING then
		airport.call_client(roleid, "cs_texas_action_rsp", errno.DATA_ERROR)
		return
	end

	local game = room.game.texas
	local player = game.player_table[roleid]
	if not player then
		airport.call_client(roleid, "cs_texas_action_rsp", errno.DATA_ERROR)
		return
	end

	local hand = game.current
	if not hand or hand.index ~= hand_idx then
		airport.call_client(roleid, "cs_texas_action_rsp", errno.PARAM_ERROR)
		return
	end

	local round = hand.rounds[#hand.rounds]
	if not round or round.index ~= round_idx then
		airport.call_client(roleid, "cs_texas_action_rsp", errno.PARAM_ERROR)
		return
	end

	local result = dealer.on_proc_action(game, player, act_type, act_chips)
	airport.call_client(roleid, "cs_texas_action_rsp", result)
end
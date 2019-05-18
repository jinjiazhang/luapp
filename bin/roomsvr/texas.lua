-- 德州玩法
module = "texas"
dealer = import("roomsvr/dealer.lua")
scorer = import("roomsvr/scorer.lua")

MIN_PLAYER_NUM = 2
MAX_PLAYER_NUM = 9

function on_create_room( room )
	log_info("on_create_room", room.roomid)
	local game = proto.create("texas_detail")
	game.seat_table = {} -- seatid -> texas_player
	game.player_table = {} -- roleid -> texas_player
	game.button = 0
	game.hands = {}
	game.current = nil

	game.broadcast = room.broadcast
	room.game.texas = game
end

function on_enter_room( room, roleid )
	log_info("on_enter_room", room.roomid, roleid)
end

function on_leave_room( room, roleid )
	log_info("on_leave_room", room.roomid, roleid)
end

function on_tick_room( room )
	-- log_info("on_tick_room", room.roomid)
end

function on_game_start( game )
	start_new_hand(game)
end

function next_seat( game, seatid )
	for i = 1, MAX_PLAYER_NUM do
		local index = (seatid + i - 1) % MAX_PLAYER_NUM + 1
		local player = game.players[index]
		if player and player.playing then
			return player.seatid
		end
	end
	return seatid
end

function start_new_hand( game )
	local hand = proto.create("texas_hand")
	hand.privacies = {}
	hand.button = next_seat(game, game.button)
	hand.start_time = app.mstime() - game.start_time * 1000
	dealer.shuffle_card(game, hand)
	start_new_round(game, hand)

	game.current = hand
	table.insert(game.hands, hand)
end

function start_new_round( game, hand )
	local round_idx = #hand.rounds
	dealer.deal_card(game, hand, round_idx)
end

function env.cs_texas_chat_req( room, roleid, flowid, content )
	if not room.get_viewer(roleid) then
		return errno.DATA_ERROR
	end
	
	room.broadcast(0, "cs_texas_chat_ntf", 0, room.roomid, roleid, content)
	return errno.SUCCESS
end

function env.cs_texas_sitdown_req( room, roleid, flowid, seatid )
	local role = room.get_viewer(roleid)
	if not role then
		return errno.DATA_ERROR
	end

	if seatid <= 0 or seatid > MAX_PLAYER_NUM then
		return errno.PARAM_ERROR
	end

	local game = room.game.texas
	if game.player_table[roleid] then
		return errno.TEXAS_HAS_SEAT
	end

	if game.seat_table[seatid] then
		return errno.TEXAS_SEAT_USED
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

	room.broadcast(roleid, "cs_texas_sitdown_ntf", 0, room.roomid, player)
	return errno.SUCCESS, player
end

function env.cs_texas_standup_req( room, roleid, flowid )
	local game = room.game.texas
	local player = game.player_table[roleid]
	if not player then
		return errno.DATA_ERROR
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

	room.broadcast(roleid, "cs_texas_standup_ntf", 0, room.roomid, seatid)
	return errno.SUCCESS
end

function env.cs_texas_start_req( room, roleid, flowid )
	local game = room.game.texas
	local player = game.player_table[roleid]
	if not player then
		return errno.DATA_ERROR
	end

	if roleid ~= room.master then
		return errno.PRIVILEGE
	end

	if room.status ~= room_status.WAITING then
		return errno.DATA_ERROR
	end

	if #game.players < MIN_PLAYER_NUM then
		return errno.TEXAS_PLAYER_NUM
	end

	room.status = room_status.PLAYING
	room.start_time = app.time()
	game.start_time = room.start_time
	room.broadcast(roleid, "cs_texas_start_ntf", 0, room.roomid)
	on_game_start(game)
	return errno.SUCCESS
end
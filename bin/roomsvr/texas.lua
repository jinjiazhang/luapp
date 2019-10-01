module = "texas"
dealer = import("roomsvr/dealer.lua")

MIN_PLAYER_NUM = 2
MAX_PLAYER_NUM = 9

function on_create_room( room )
	log_info("on_create_room", room.roomid)
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
	log_info("on_enter_room", room.roomid, roleid)
end

function on_leave_room( room, roleid )
	log_info("on_leave_room", room.roomid, roleid)
end

function on_tick_room( room )
	-- log_info("on_tick_room", room.roomid)
	if room.status ~= room_status.PLAYING then
		return
	end

	local game = room.game.texas
	check_new_hand(game)
	dealer.tick_timeout(game)
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

function check_new_hand( game )
	if not game.current then
		start_new_hand(game)
	end
end

function start_new_hand( game )
	local option = game.option
	local hand = proto.create("texas_hand")
	hand.privacies = {}
	hand.index = #game.hands + 1
	hand.button = next_seat(game, game.button)
	hand.start_time = app.mstime()
	game.current = hand
	table.insert(game.hands, hand)

	start_new_round(game)
	dealer.apply_player(game)
	dealer.ante_action(game)
	dealer.blind_action(game)
	game.broadcast(0,"cs_texas_hand_ntf", game.roomid, hand)

	dealer.shuffle_card(game)
	dealer.deal_privacy(game)
end

function start_new_round( game, card_num, notify )
	local hand = game.current
	local round = proto.create("texas_round")
	round.index = #hand.rounds + 1
	round.start_time = app.mstime() - hand.start_time
	hand.current = round
	table.insert(hand.rounds, round)

	if card_num and card_num > 0 then
		dealer.deal_community(game, card_num)
	end

	if notify then
		game.broadcast(0,"cs_texas_round_ntf", game.roomid, hand.index, round)
	end
end

function env.cs_texas_chat_req( room, roleid, content )
	if not room.get_viewer(roleid) then
		return errno.DATA_ERROR
	end

	room.broadcast(0,"cs_texas_chat_ntf", room.roomid, roleid, content)
	return errno.SUCCESS
end

function env.cs_texas_sitdown_req( room, roleid, seatid )
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

	room.broadcast(roleid, "cs_texas_sitdown_ntf", room.roomid, player)
	return errno.SUCCESS, player
end

function env.cs_texas_standup_req( room, roleid )
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

	room.broadcast(roleid, "cs_texas_standup_ntf", room.roomid, seatid)
	return errno.SUCCESS
end

function env.cs_texas_start_req( room, roleid )
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

	game.button = player.seatid
	room.status = room_status.PLAYING
	room.start_time = app.time()
	room.broadcast(roleid, "cs_texas_start_ntf", room.roomid)
	return errno.SUCCESS
end

function env.cs_texas_action_req( room, roleid, hand_idx, round_idx, act_type, act_chips )
	if room.status ~= room_status.PLAYING then
		return errno.DATA_ERROR
	end

	local game = room.game.texas
	local player = game.player_table[roleid]
	if not player then
		return errno.DATA_ERROR
	end

	local hand = game.current
	if not hand or hand.index ~= hand_idx then
		return errno.PARAM_ERROR
	end

	local round = hand.rounds[#hand.rounds]
	if not round or round.index ~= round_idx then
		return errno.PARAM_ERROR
	end

	return dealer.proc_action(game, player, act_type, act_chips)
end
banker = import("roomsvr/texas_banker.lua")
-- todo1: all player allin
-- todo2: second times call

function tick_game( game )
	if not game.current then
		start_new_hand(game)
	else
		tick_timeout(game)
	end
end

function tick_timeout( game )
	local hand = game.current
	if hand.status == texas_status.INIT then
		if app.mstime() - hand.init_time >= 3000 then
			hand.status = texas_status.PREFLOP
			hand.action_time = app.mstime()
			hand.action_seatid = hand.ingame_seats[1].seatid
			notify_cur_turn(game)
		end
	elseif hand.status == texas_status.SETTLING then
		if app.mstime() - hand.settle_time >= 5000 then
			hand.status = texas_status.FINISH
			finish_cur_hand(game)
		end
	else
		if hand.action_seatid == 0 then
			round_move_turn(game)
			hand.action_time = app.mstime()
			hand.action_seatid = hand.ingame_seats[1].seatid
			notify_cur_turn(game)
		end

		if app.mstime() - hand.action_time >= 15000 then
			on_timeout_action(game)
		end
	end
end

function start_new_hand( game )
	local option = game.option
	local hand = proto.create("texas_hand")
	hand.index = #game.hands + 1
	hand.button = game.button
	hand.start_time = app.time()
	game.current = hand
	table.insert(game.hands, hand)

	hand.status = texas_status.INIT
	hand.init_time = app.mstime()
	hand.settle_time = 0
	hand.action_time = 0
	hand.action_seatid = 0

	hand.stock_cards = shuffle_card()
	hand.shared_cards = {}
	hand.ingame_seats = {}
	init_ingame_seats(game)
	hand.ingame_count = #hand.ingame_seats
	hand.incall_count = 0

	start_new_round(game, false)
	on_ante_action(game)
	on_blind_action(game)
	game.broadcast(0,"cs_texas_hand_ntf", game.roomid, hand)

	deal_hole_card(game)
end

function finish_cur_hand( game )
	local hand = game.current
	game.button = hand.button
	game.current = nil
end

function init_ingame_seats( game )
	local hand = game.current
	local deal_order = {}
	for seatid = 1, texas.MAX_PLAYER_NUM do
		local player = game.seat_table[seatid]
		if player ~= nil then
			table.insert(deal_order, seatid)
		end
	end

	-- start from small blind
	while deal_order[#deal_order] ~= hand.button do
		local seatid = table.remove(deal_order, 1)
		table.insert(deal_order, seatid)
	end

	for _, seatid in ipairs(deal_order) do
		local player = game.seat_table[seatid]
		local seat = proto.create("texas_seat")
		seat.seatid = seatid
		seat.roleid = player.roleid
		seat.card1 = 0
		seat.card2 = 0
		seat.show_flag = 0
		seat.is_fold = false
		seat.is_allin = false
		table.insert(hand.ingame_seats, seat)
	end
end

function start_new_round( game, notify )
	local hand = game.current
	local round = proto.create("texas_round")
	round.index = #hand.rounds + 1
	round.game_time = app.mstime() - hand.init_time
	hand.current = round
	table.insert(hand.rounds, round)

	hand.first_bet = 0
	hand.max_raise = 0

	if notify then
		game.broadcast(0, "cs_texas_round_ntf", game.roomid, hand.index, round)
	end
end

function shuffle_card( )
	local cards = {
		01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13,
		14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
		27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52
	}

	local count = #cards
	for i = 1, count do
		local j = math.random(1, count)
		cards[i], cards[j] = cards[j], cards[i]
	end

	return cards
end

function deal_hole_card( game )
	local hand = game.current
	local cards = hand.stock_cards
	for _, seat in ipairs(hand.ingame_seats) do
		seat.card1 = table.remove(cards)
		seat.card2 = table.remove(cards)
		local deal_cards = { seat.card1, seat.card2 }
		airport.call_client(seat.roleid, "cs_texas_deal_ntf", game.roomid, hand.index, seat.seatid, deal_cards)
		game.broadcast(seat.roleid, "cs_texas_deal_ntf", game.roomid, hand.index, seat.seatid, { 0, 0 })
	end
end

function deal_shared_card( game, count )
	local hand = game.current
	local round = hand.current
	local cards = hand.stock_cards
	local deal_cards = {}
	for i = 1, count do
		local card = table.remove(cards)
		table.insert(deal_cards, card)
		table.insert(hand.shared_cards, card)
	end

	game.broadcast(0, "cs_texas_deal_ntf", game.roomid, hand.index, 0, deal_cards)
end

function accept_action( game, seatid, type, chips, notify)
	local hand = game.current
	local round = hand.current
	local action = {
		seatid = seatid,
		game_time = app.mstime() - hand.init_time,
		act_type = type,
		act_chips = chips
	}
	table.insert(round.actions, action)
	hand.action_time = app.mstime()
	hand.action_seatid = 0

	if notify then
		game.broadcast(0, "cs_texas_action_ntf", game.roomid, hand.index, round.index, action)
	end
end

function notify_cur_turn( game )
	local hand = game.current
	if hand.status >= texas_status.SETTLING then
		game.broadcast(0, "cs_texas_turn_ntf", game.roomid, hand.index, 0)
	else
		local trun_seat = hand.ingame_seats[1]
		game.broadcast(0, "cs_texas_turn_ntf", game.roomid, hand.index, trun_seat.seatid)
	end
end

function seat_move_turn( game )
	local hand = game.current
	for times = 1, #hand.ingame_seats do
		local seat = table.remove(hand.ingame_seats, 1)
		table.insert(hand.ingame_seats, seat)
		
		if seat.is_allin then
			hand.incall_count = hand.incall_count + 1
		end
		
		if not seat.is_fold then
			break
		end
	end
end

function round_move_turn( game )
	local hand = game.current
	if hand.ingame_count == 1 then
		hand.status = texas_status.SETTLING
		hand.settle_time = app.mstime()
		banker.settle_cur_hand(game)
		return
	end

	seat_move_turn(game)
	if hand.incall_count < hand.ingame_count then
		return
	end

	-- finish current round
	if hand.status == texas_status.PREFLOP then
		hand.status = texas_status.FLOP
		start_new_round(game, true)
		deal_shared_card(game, 3)
	elseif hand.status == texas_status.FLOP then
		hand.status = texas_status.TURN
		start_new_round(game, true)
		deal_shared_card(game, 1)
	elseif hand.status == texas_status.TURN then
		hand.status = texas_status.RIVER
		start_new_round(game, true)
		deal_shared_card(game, 1)
	elseif hand.status == texas_status.RIVER then
		hand.status = texas_status.SETTLING
		hand.settle_time = app.mstime()
		banker.settle_cur_hand(game)
		return
	end

	-- reset for new round
	hand.incall_count = 0

	while hand.ingame_seats[1].seatid ~= hand.button do
		local seat = table.remove(hand.ingame_seats, 1)
		table.insert(hand.ingame_seats, seat)
	end

	seat_move_turn(game)
end

function on_proc_action( game, player, type, chips )
	local hand = game.current
	log_info("on_proc_action", player.roleid, hand.action_seatid)
	if player.seatid ~= hand.action_seatid then
		return errno.TEXAS_TURN_ERROR
	end

	-- for robot test
	type = revise_action_type(game, player, chips)

	if type == action_type.BET then
		return on_bet_action(game, player, chips)
	elseif type == action_type.CALL then
		return on_call_action(game, player, chips)
	elseif type == action_type.FOLD then
		return on_fold_action(game, player, chips)
	elseif type == action_type.CHECK then
		return on_check_action(game, player, chips)
	elseif type == action_type.RAISE then
		return on_raise_action(game, player, chips)
	elseif type == action_type.RE_RAISE then
		return on_reraise_action(game, player, chips)
	elseif type == action_type.ALL_IN then
		return on_allin_action(game, player, chips)
	else
		return errno.PARAM_ERROR
	end
end

function on_timeout_action( game )

end

function on_ante_action( game )
	local ante_chips = game.option.ante
	if ante_chips <= 0 then
		return errno.SUCCESS
	end

	local hand = game.current
	for _, seat in ipairs(hand.ingame_seats) do
		accept_action(game, seat.seatid, action_type.ANTE, ante_chips, false)
	end
	return errno.SUCCESS
end

function on_blind_action( game )
	local hand = game.current
	local small_blind = game.option.small_blind
	local small_seat = table.remove(hand.ingame_seats, 1)
	accept_action(game, small_seat.seatid, action_type.SMALL_BLIND, small_blind, false)
	table.insert(hand.ingame_seats, small_seat)

	local big_blind = game.option.big_blind
	local big_seat = table.remove(hand.ingame_seats, 1)
	accept_action(game, big_seat.seatid, action_type.BIG_BLIND, big_blind, false)
	table.insert(hand.ingame_seats, big_seat)

	hand.incall_count = 1
	return errno.SUCCESS
end

function on_bet_action( game, player, chips )
	local hand = game.current
	if hand.first_bet > 0 then
		return errno.ACTION_ERROR
	end

	if chips <= 0 or chips >= player.chips then
		return errno.PARAM_ERROR
	end

	accept_action(game, player.seatid, action_type.BET, chips, true)
	hand.first_bet = chips
	hand.max_raise = chips
	hand.incall_count = 1
	return errno.SUCCESS
end

function on_call_action( game, player, chips )
	local hand = game.current
	if hand.first_bet <= 0 then
		return errno.ACTION_ERROR
	end

	if chips ~= hand.max_raise or chips >= player.chips then
		return errno.PARAM_ERROR
	end

	accept_action(game, player.seatid, action_type.CALL, chips, true)
	hand.incall_count = hand.incall_count + 1
	return errno.SUCCESS
end

function on_fold_action( game, player, chips )
	local hand = game.current
	accept_action(game, player.seatid, action_type.FOLD, 0, true)
	hand.ingame_seats[1].is_fold = true
	hand.ingame_count = hand.ingame_count - 1
	return errno.SUCCESS
end

function on_check_action( game, player, chips )
	local hand = game.current
	if hand.first_bet > 0 then
		return errno.ACTION_ERROR
	end

	accept_action(game, player.seatid, action_type.CHECK, 0, true)
	hand.incall_count = hand.incall_count + 1
	return errno.SUCCESS
end

function on_raise_action( game, player, chips )
	local hand = game.current
	if hand.first_bet <= 0 or hand.max_raise > hand.first_bet then
		return errno.ACTION_ERROR
	end

	if chips <= hand.max_raise or chips >= player.chips then
		return errno.PARAM_ERROR
	end

	accept_action(game, player.seatid, action_type.RAISE, chips, true)
	hand.max_raise = chips
	hand.incall_count = 1
	return errno.SUCCESS
end

function on_reraise_action( game, player, chips )
	local hand = game.current
	if hand.first_bet <= 0 or hand.max_raise <= hand.first_bet then
		return errno.ACTION_ERROR
	end

	if chips <= hand.max_raise or chips >= player.chips then
		return errno.PARAM_ERROR
	end

	accept_action(game, player.seatid, action_type.RERAISE, chips, true)
	hand.max_raise = chips
	hand.incall_count = 1
	return errno.SUCCESS
end

function on_allin_action( game, player, chips )
	local hand = game.current
	if chips <= 0 or chips ~= player.chips then
		return errno.PARAM_ERROR
	end

	accept_action(game, player.seatid, action_type.ALL_IN, chips, true)
	hand.ingame_seats[1].is_allin = true
	if chips > hand.max_raise then
		hand.max_raise = chips
		hand.incall_count = 1
	else
		hand.incall_count = hand.incall_count + 1
	end
	return errno.SUCCESS
end

function revise_action_type( game, player, chips )
	local hand = game.current
	if chips < 0 then
		return action_type.FOLD
	elseif chips == 0 then
		return action_type.CHECK
	elseif chips == player.chips then
		return action_type.ALL_IN
	elseif chips == hand.max_raise then
		return action_type.CALL
	elseif chips > hand.max_raise then
		if hand.first_bet == 0 then
			return action_type.BET
		else
			if hand.max_raise == hand.first_bet then
				return action_type.RAISE
			else
				return action_type.RERAISE
			end
		end
	else
		return action.FOLD
	end
end
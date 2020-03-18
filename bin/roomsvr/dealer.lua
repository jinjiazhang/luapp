
function tick_game( game )
	if not game.current then
		start_new_hand(game)
	else
		tick_timeout(game)
	end
end

function tick_timeout( game )

end

function start_new_hand( game )
	local option = game.option
	local hand = proto.create("texas_hand")
	hand.index = #game.hands + 1
	hand.button = game.button
	hand.start_time = app.time()
	game.current = hand
	table.insert(game.hands, hand)

	hand.stock_cards = shuffle_card()
	hand.shared_cards = {}
	hand.ingame_seats = {}
	hand.ingame_count = 0
	init_ingame_seats(game)

	start_new_round(game)
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

	hand.ingame_count = #deal_order
	for _, seatid in ipairs(deal_order) do
		local player = game.seat_table[seatid]
		local seat = proto.create("texas_seat")
		seat.seatid = seatid
		seat.roleid = player.roleid
		seat.card1 = 0
		seat.card2 = 0
		seat.show_flag = 0
		table.insert(hand.ingame_seats, seat)
	end
end

function start_new_round( game )
	local hand = game.current
	local round = proto.create("texas_round")
	round.index = #hand.rounds + 1
	round.game_time = app.mstime() - hand.start_time * 1000
	hand.current = round
	table.insert(hand.rounds, round)
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
	for i = 1, count do
		local card = table.remove(cards)
		table.insert(round.cards, card)
		table.insert(hand.shared_cards, card)
		game.broadcast(0, "cs_texas_deal_ntf", game.roomid, hand.index, 0, deal_cards)
	end
end

function apply_action( game, seatid, type, chips, notify)
	local hand = game.current
	local round = hand.current
	local action = {
		seatid = seatid,
		game_time = app.mstime() - hand.start_time * 1000,
		act_type = type,
		act_chips = chips
	}
	table.insert(round.actions, action)

	if notify then
		game.broadcast(0, "cs_texas_action_ntf", game.roomid, hand.index, round.index, action)
	end
end

function on_proc_action( game, player, type, chips )
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

function on_ante_action( game )
	local ante_chips = game.option.ante
	if ante_chips <= 0 then
		return
	end

	local hand = game.current
	for _, seat in ipairs(hand.ingame_seats) do
		apply_action(game, seat.seatid, action_type.ANTE, ante_chips, false)
	end
end

function on_blind_action( game )
	local hand = game.current
	local small_blind = game.option.small_blind
	local small_seat = table.remove(hand.ingame_seats, 1)
	apply_action(game, small_seat.seatid, action_type.SMALL_BLIND, small_blind, false)
	table.insert(hand.ingame_seats, small_seat)

	local big_blind = game.option.big_blind
	local big_seat = table.remove(hand.ingame_seats, 1)
	apply_action(game, big_seat.seatid, action_type.BIG_BLIND, big_blind, false)
	table.insert(hand.ingame_seats, big_seat)
end

function on_bet_action( game, player, chips )
	if chips <= 0 then
		return errno.PARAM_ERROR
	end

	apply_action(game, player.seatid, action_type.BET, chips, true)
	return errno.SUCCESS
end

function on_call_action( game, player, chips )
	if chips <= 0 then
		return errno.PARAM_ERROR
	end

	apply_action(game, player.seatid, action_type.CALL, chips, true)
	return errno.SUCCESS
end

function on_fold_action( game, player, chips )
	apply_action(game, player.seatid, action_type.FOLD, 0, true)
	return errno.SUCCESS
end

function on_check_action( game, player, chips )
	apply_action(game, player.seatid, action_type.CHECK, 0, true)
	return errno.SUCCESS
end

function on_raise_action( game, player, chips )
	if chips <= 0 then
		return errno.PARAM_ERROR
	end

	apply_action(game, player.seatid, action_type.RAISE, chips, true)
	return errno.SUCCESS
end

function on_reraise_action( game, player, chips )
	if chips <= 0 then
		return errno.PARAM_ERROR
	end

	apply_action(game, player.seatid, action_type.RERAISE, chips, true)
	return errno.SUCCESS
end

function on_allin_action( game, player, chips )
	if chips <= 0 then
		return errno.PARAM_ERROR
	end

	apply_action(game, player.seatid, action_type.ALL_IN, chips, true)
	return errno.SUCCESS
end
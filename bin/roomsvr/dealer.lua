
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
	hand.shared_cards = {}
	hand.hole_cards = {}
	hand.deal_order = {}
	hand.stock_cards = nil

	hand.index = #game.hands + 1
	hand.button = game.button
	hand.start_time = app.time()
	game.current = hand
	table.insert(game.hands, hand)

	apply_deal_order(game)
	start_new_round(game)
	on_ante_action(game)
	on_blind_action(game)
	game.broadcast(0,"cs_texas_hand_ntf", game.roomid, hand)

	shuffle_card(game)
	deal_hole_card(game)
end

function finish_cur_hand( game )
	local hand = game.current
	game.button = hand.button
	game.current = nil
end

function start_new_round( game )
	local hand = game.current
	local round = proto.create("texas_round")
	round.index = #hand.rounds + 1
	round.game_time = app.mstime() - hand.start_time * 1000
	hand.current = round
	table.insert(hand.rounds, round)
end

function apply_deal_order( game )
	local hand = game.current
	for seatid = 1, texas.MAX_PLAYER_NUM do
		local player = game.seat_table[seatid]
		if player ~= nil then
			table.insert(hand.deal_order, seatid)
		end
	end

	for times = 1, texas.MAX_PLAYER_NUM do
		if hand.deal_order[1] == hand.button then
			break
		end

		local seatid = table.remove(hand.deal_order, 1)
		table.insert(hand.deal_order, seatid)
	end
end

function shuffle_card( game )
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

	local hand = game.current
	hand.stock_cards = cards
end

function deal_hole_card( game )
	local hand = game.current
	local cards = hand.stock_cards
	for _, seatid in ipairs(hand.deal_order) do
		local player = game.seat_table[seatid]
		assert(player ~= nil)

		local hole = proto.create("texas_hole")
		hole.seatid = seatid
		hole.card1 = table.remove(cards)
		hole.card2 = table.remove(cards)
		hole.show_flag = 0
		table.insert(hand.hole_cards, hole)

		local deal_cards = { hole.card1, hole.card2 }
		airport.call_client(player.roleid, "cs_texas_deal_ntf", game.roomid, hand.index, deal_cards)
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
	for _, seatid in ipairs(hand.deal_order) do
		apply_action( game, seatid, action_type.ANTE, ante_chips, false)
	end
end

function on_blind_action( game )
	local small_blind = game.option.small_blind
	local big_blind = game.option.big_blind
	local hand = game.current
	apply_action( game, hand.deal_order[1], action_type.SMALL_BLIND, small_blind, false)
	apply_action( game, hand.deal_order[2], action_type.BIG_BLIND, big_blind, false)
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
	apply_action( game, player.seatid, action_type.CHECK, 0, true)
	return errno.SUCCESS
end

function on_raise_action( game, player, chips )
	if chips <= 0 then
		return errno.PARAM_ERROR
	end

	apply_action( game, player.seatid, action_type.RAISE, chips, true)
	return errno.SUCCESS
end

function on_reraise_action( game, player, chips )
	if chips <= 0 then
		return errno.PARAM_ERROR
	end

	apply_action( game, player.seatid, action_type.RERAISE, chips, true)
	return errno.SUCCESS
end

function on_allin_action( game, player, chips )
	if chips <= 0 then
		return errno.PARAM_ERROR
	end

	apply_action( game, player.seatid, action_type.ALL_IN, chips, true)
	return errno.SUCCESS
end
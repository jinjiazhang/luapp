
function apply_player( game )
	local hand = game.current
	hand.seatids = {}
	local seatid = game.button
	for i = 1, texas.MAX_PLAYER_NUM do
		seatid = texas.next_seat(game, seatid)
		if game.seat_table[seatid] then
			table.insert(hand.seatids, seatid)
		end

		if seatid == game.button then
			break
		end
	end
	log_info("seatids", app.tostring(hand.seatids))
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
	hand.cards = cards
end

function deal_privacy( game )
	local hand = game.current
	local cards = hand.cards
	for _, seatid in ipairs(hand.seatids) do
		local card1 = table.remove(cards)
		local card2 = table.remove(cards)
		local privacy = proto.create("texas_privacy")
		privacy.seatid = seatid
		privacy.hole_card1 = card1
		privacy.hole_card2 = card2
		privacy.show_flag = 0
		table.insert(hand.privacies, privacy)

		local player = game.seat_table[seatid]
		assert(player ~= nil)
		local deal_cards = { card1, card2 }
		airport.call_client(player.roleid, "cs_texas_deal_ntf", 0, game.roomid, hand.index, deal_cards)
	end
end

function deal_community( game, count )
	local hand = game.current
	local round = hand.current
	local cards = hand.cards
	for i = 1, count do
		local card = table.remove(cards)
		table.insert(round.cards, card)
		table.insert(hand.communities, card)
	end
end

function tick_timeout( game )

end

function proc_action( game, player, type, chips )
	if type == action_type.BET then
		return bet_action(game, player, chips)
	elseif type == action_type.CALL then
		return call_action(game, player, chips)
	elseif type == action_type.FOLD then
		return fold_action(game, player, chips)
	elseif type == action_type.CHECK then
		return check_action(game, player, chips)
	elseif type == action_type.RAISE then
		return raise_action(game, player, chips)
	elseif type == action_type.RE_RAISE then
		return reraise_action(game, player, chips)
	elseif type == action_type.ALL_IN then
		return allin_action(game, player, chips)
	else
		return errno.PARAM_ERROR
	end
end

function apply_action( game, seatid, type, chips, notify)
	local hand = game.current
	local round = hand.current
	local action = {
		seatid = seatid,
		act_time = app.mstime() - hand.start_time,
		act_type = type,
		act_chips = chips
	}

	table.insert(round.actions, action)
end

function ante_action( game )
	log_info("ante_action", app.tostring(game.option))
	local ante_chips = game.option.ante
	if ante_chips <= 0 then
		return
	end

	local hand = game.current
	for _, seatid in ipairs(hand.seatids) do
		apply_action( game, seatid, action_type.ANTE, ante_chips, false)
	end
end

function blind_action( game )
	local small_blind = game.option.small_blind
	local big_blind = game.option.big_blind
	local hand = game.current
	apply_action( game, hand.seatids[1], action_type.SMALL_BLIND, small_blind, false)
	apply_action( game, hand.seatids[2], action_type.BIG_BLIND, big_blind, false)
end

function bet_action( game, player, chips )
	if chips <= 0 then
		return errno.PARAM_ERROR
	end

	apply_action(game, player.seatid, action_type.BET, chips, true)
	return errno.SUCCESS
end

function call_action( game, player, chips )
	if chips <= 0 then
		return errno.PARAM_ERROR
	end

	apply_action(game, player.seatid, action_type.CALL, chips, true)
	return errno.SUCCESS
end

function fold_action( game, player, chips )
	apply_action(game, player.seatid, action_type.FOLD, 0, true)
	return errno.SUCCESS
end

function check_action( game, player, chips )
	apply_action( game, player.seatid, action_type.CHECK, 0, true)
	return errno.SUCCESS
end

function raise_action( game, player, chips )
	if chips <= 0 then
		return errno.PARAM_ERROR
	end

	apply_action( game, player.seatid, action_type.RAISE, chips, true)
	return errno.SUCCESS
end

function reraise_action( game, player, chips )
	if chips <= 0 then
		return errno.PARAM_ERROR
	end

	apply_action( game, player.seatid, action_type.RERAISE, chips, true)
	return errno.SUCCESS
end

function allin_action( game, player, chips )
	if chips <= 0 then
		return errno.PARAM_ERROR
	end

	apply_action( game, player.seatid, action_type.ALL_IN, chips, true)
	return errno.SUCCESS
end
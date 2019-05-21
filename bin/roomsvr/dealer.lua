-- 德州玩法
MIN_PLAYER_NUM = 2
MAX_PLAYER_NUM = 9

function shuffle_card( game, hand )
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

	hand.cards = cards
end

function deal_card( game, hand, round )
	if round.index == 1 then
		deal_privacy_card(game, hand, round)
	elseif round.index == 2 then
		deal_flop_card(game, hand, round)
	elseif round.index == 3 then
		deal_turn_card(game, hand, round)
	elseif round.index == 4 then
		deal_river_card(game, hand, round)
	else
		log_error("deal_card round index invalid", round.index)
	end
end

function deal_privacy_card( game, hand, round )
	local cards = hand.cards
	local seatid = game.button
	for i = 1, MAX_PLAYER_NUM do
		local card1 = table.remove(cards)
		local card2 = table.remove(cards)
		local privacy = proto.create("texas_privacy")
		privacy.seatid = seatid
		privacy.hole_card1 = card1
		privacy.hole_card2 = card2
		privacy.show_flag = 0
		table.insert(hand.privacies, privacy)

		local player = game.seat_table[seatid]
		if player then
			local deal_cards = { card1, card2 }
			airport.call_client(player.roleid, "cs_texas_deal_ntf", 0, game.roomid, hand.index, round.index, round.start_time, deal_cards)
		end
		
		seatid = texas.next_seat(game, seatid)
		if seatid == game.button then
			break
		end
	end
end

function deal_flop_card( game, hand, round )
	local cards = hand.cards
	local card1 = table.remove(cards)
	local card2 = table.remove(cards)
	local card3 = table.remove(cards)
	table.insert(hand.communities, card1)
	table.insert(hand.communities, card2)
	table.insert(hand.communities, card3)

	local deal_cards = { card1, card2, card3 }
	game.broadcast(0, "cs_texas_deal_ntf", 0, game.roomid, hand.index, round.index, round.start_time, deal_cards)
end

function deal_turn_card( game, hand, round )
	local cards = hand.cards
	table.remove(cards)
	local card = table.remove(cards)
	table.insert(hand.communities, card)

	local deal_cards = { card }
	game.broadcast(0, "cs_texas_deal_ntf", 0, game.roomid, hand.index, round.index, round.start_time, deal_cards)
end

function deal_river_card( game, hand, round )
	local cards = hand.cards
	table.remove(cards)
	local card = table.remove(cards)
	table.insert(hand.communities, card)

	local deal_cards = { card }
	game.broadcast(0, "cs_texas_deal_ntf", 0, game.roomid, hand.index, round.index, round.start_time, deal_cards)
end
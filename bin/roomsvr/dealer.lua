
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
	local seatid = game.button
	for i = 1, texas.MAX_PLAYER_NUM do
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
		
		seatid = texas.next_seat(game, seatid)
		if seatid == game.button then
			break
		end
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

function proc_action( game, player, act_type, act_chips )

end

function ante_action( game )

end

function blind_action( game )

end
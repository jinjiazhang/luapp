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

function deal_card( game, hand, round_idx )
	if round_idx == 0 then
		deal_hole_card(game, hand)
	elseif round_idx == 1 then
		deal_flop_card(game, hand)
	elseif round_idx == 2 then
		deal_trun_card(game, hand)
	elseif round_idx == 3 then
		deal_river_card(game, hand)
	else
		log_error("deal_card round index invalid", round_idx)
	end
end

function deal_hole_card( game, hand )

end

function deal_flop_card( game, hand )

end

function deal_trun_card( game, hand )

end

function deal_river_card( game, hand )
	local cards = hand.cards
	table.remove(cards, 1)
	
end
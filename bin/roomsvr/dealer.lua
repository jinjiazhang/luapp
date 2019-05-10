-- 德州玩法
MIN_PLAYER_NUM = 2
MAX_PLAYER_NUM = 9

function shuffle_card( game, round )
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

	round.cards = cards
end

function deal_card( game, round, turn_idx )
	if turn_idx == 0 then
	end
end
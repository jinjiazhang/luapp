judge = import("gamesvr/texas_judge.lua")

function settle_cur_hand( game )
    local hand = game.current
    local incall_list = {}
    for _, seat in ipairs(hand.ingame_seats) do
        if not seat.is_fold then
            table.insert(incall_list, seat)
        end
    end

    if #incall_list == 1 then
        return simple_settle(game, incall_list)
    else
        return complex_settle(game, incall_list)
    end
end

function simple_settle( game, incall_list )
    local hand = game.current
    local winner = incall_list[1]
    local prize_table = {}
    prize_table[winner.seatid] = hand.pot_chips
    return settle_prize(game, prize_table)
end

function complex_settle( game, incall_list )
    local hand = game.current
    local score_table = {}
    local card_table = {}
    for _, seat in ipairs(incall_list) do
        local cards = {seat.card1, seat.card2}
        for _, card in ipairs(hand.shared_cards) do
            table.insert(cards, card)
        end        
        local max_score, max_cards = judge.calc_score_7(cards)
        score_table[seat.seatid] = max_score
        card_table[seat.seatid] = max_cards
    end

    local prize_table = assign_prize(game, score_table)
    return settle_prize(game, prize_table, card_table)
end

function assign_prize( game, score_table )
    local hand = game.current
    local max_score = 0
    local winner_seatid = 0
    for seatid, score in pairs(score_table) do
        if score > max_score then
            max_score = score
            winner_seatid = seatid
        end
    end

    local prize_table = {}
    prize_table[winner_seatid] = hand.pot_chips
    return prize_table
end

function settle_prize( game, prize_table, card_table )
    for seatid, prize in pairs(prize_table) do
        local player = game.seat_table[seatid]
        player.chips = player.chips + prize
    end
end
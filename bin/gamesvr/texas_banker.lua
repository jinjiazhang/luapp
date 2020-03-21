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
    local seatid = incall_list[1].seatid
    local prize_table = {{[seatid] = hand.pot_chips}}
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

    local prize_table = complex_assign(game, score_table)
    return settle_prize(game, prize_table, card_table)
end

function filter_winner( score_table )
    local seatids = {}
    local max_score = 0
    for seatid, score in pairs(score_table) do
        if score > max_score then
            max_score = score
            seatids = {seatid}
        elseif score == max_score then
            table.insert(seatids, seatid)
        end
    end
    return seatids
end

function calc_deal_turn( game, seatid )
    local button = game.current.button
    if seatid > button then
        return seatid - button
    else
        return seatid + texas.MAX_PLAYER_NUM - button
    end
end

function complex_assign( game, score_table )
    local hand = game.current
    local prize_table = {}
    while next(score_table) do
        local seatids = filter_winner(score_table)
        local prizes = assign_prize(game, seatids)
        table.insert(prize_table, prizes)

        for _, seatid in ipairs(seatids) do
            if hand.hand_chips[seatid] <= 0 then
                score_table[seatid] = nil
            end
        end
    end

    -- todo: fold seat has chips left
    assert(hand.pot_chips == 0)
    return prize_table
end

function assign_prize( game, seatids )
    local hand = game.current
    local min_chips = nil
    for _, seatid in ipairs(seatids) do
        local bet_chips = hand.hand_chips[seatid]
        if min_chips == nil or bet_chips < min_chips then
            min_chips = bet_chips
        end
    end

    local prize_pot = 0
    for seatid, bet_chips in pairs(hand.hand_chips) do
        local cost_chips = math.min(bet_chips, min_chips)
        prize_pot = prize_pot + cost_chips
        hand.pot_chips = hand.pot_chips - cost_chips
        hand.hand_chips[seatid] = bet_chips - cost_chips 
    end

    if prize_pot == 0 then
        return nil
    end

    local prizes = {}
    local prize = prize_pot // #seatids
    for _, seatid in ipairs(seatids) do
        prizes[seatid] = prize
        prize_pot = prize_pot - prize
    end

    if prize_pot > 0 then -- odd chips
        table.sort(seatids, function(a, b)
            return calc_deal_turn(a) < calc_deal_turn(b)
        end)

        for i = 1, prize_pot do
            local seatid = seatids[i]
            prizes[seatid] = prizes[seatid] + 1 
        end
    end
    return prizes
end

function settle_prize( game, prize_table, card_table )
    log_info("settle_prize", app.tostring(prize_table))
    local hand = game.current
    for _, prizes in ipairs(prize_table) do
        for seatid, prize in pairs(prizes) do
            local player = game.seat_table[seatid]
            player.chips = player.chips + prize
        end
    end
end
card_names = {
    'A♠', '2♠', '3♠', '4♠', '5♠', '6♠', '7♠', '8♠', '9♠', '10♠', 'J♠', 'Q♠', 'K♠',
    'A♥', '2♥', '3♥', '4♥', '5♥', '6♥', '7♥', '8♥', '9♥', '10♥', 'J♥', 'Q♥', 'K♥',
    'A♣', '2♣', '3♣', '4♣', '5♣', '6♣', '7♣', '8♣', '9♣', '10♣', 'J♣', 'Q♣', 'K♣',
    'A♦', '2♦', '3♦', '4♦', '5♦', '6♦', '7♦', '8♦', '9♦', '10♦', 'J♦', 'Q♦', 'K♦',
}

combs_7_5 = {
    {1, 2, 3, 4, 5}, {1, 2, 3, 4, 6}, {1, 2, 3, 4, 7},
    {1, 2, 3, 5, 6}, {1, 2, 3, 5, 7}, {1, 2, 3, 6, 7},
    {1, 2, 4, 5, 6}, {1, 2, 4, 5, 7}, {1, 2, 4, 6, 7},
    {1, 2, 5, 6, 7}, {1, 3, 4, 5, 6}, {1, 3, 4, 5, 7},
    {1, 3, 4, 6, 7}, {1, 3, 5, 6, 7}, {1, 4, 5, 6, 7},
    {2, 3, 4, 5, 6}, {2, 3, 4, 5, 7}, {2, 3, 4, 6, 7},
    {2, 3, 5, 6, 7}, {2, 4, 5, 6, 7}, {3, 4, 5, 6, 7},
}

texas_rank = texas_rank or {
    ["HIGH_CARD"]           = 1,
    ["ONE_PAIR"]            = 2,
    ["TWO_PAIR"]            = 3,
    ["THREE"]               = 4,
    ["STRAIGHT"]            = 5,
    ["FLUSH"]               = 6,
    ["FULLHOUSE"]           = 7,
    ["FOUR"]                = 8,
    ["STRAIGHT_FLUSH"]      = 9,
    ["ROYAL_FLUSH"]         = 10,
}

function card_color( card )
    return (card - 1) // 13 + 1
end

function card_number( card )
    local number = (card - 1) % 13 + 1
    return number == 1 and 14 or number
end

function card_string( cards )
    local names = {}
    for _, card in ipairs(cards) do
        table.insert(names, card_names[card])
    end

    return table.concat(names, " ")
end

function score_rank( score )
    return score >> 20
end

function calc_score_7( cards )
    local max_score = 0
    local max_cards = nil

    for _, comb in ipairs(combs_7_5) do
        local selected = {}
        for _, index in ipairs(comb) do
            table.insert(selected, cards[index])
        end

        local score = calc_score_5(selected)
        if score > max_score then
            max_score = score
            max_cards = selected
        end
    end
    return max_score, max_cards
end

function calc_score_5( cards )
    table.sort(cards, function(card1, card2)
        return card_number(card1) > card_number(card2)
    end)

    local n1 = card_number(cards[1])
    local n2 = card_number(cards[2])
    local n3 = card_number(cards[3])
    local n4 = card_number(cards[4])
    local n5 = card_number(cards[5])

    local d1 = n1 - n2
    local d2 = n2 - n3
    local d3 = n3 - n4
    local d4 = n4 - n5

    if d2 == 0 and d3 == 0 then
        if d1 == 0 then
            -- XXXXM
        elseif d4 == 0 then
            -- MXXXX -> XXXXM
            table.insert(cards, table.remove(cards, 1))
        else 
            -- MXXXN -> XXXMN
            table.insert(cards, 4, table.remove(cards, 1))
        end
    elseif d1 == 0 and d2 == 0 then
        -- XXXMN, XXXMM
    elseif d3 == 0 and d4 == 0 then
        -- MNXXX -> XXXMN
        table.insert(cards, table.remove(cards, 1))
        table.insert(cards, table.remove(cards, 1))
    elseif d1 == 0 and d3 == 0 then
        -- XXYYM
    elseif d1 == 0 and d4 == 0 then
        -- XXMYY -> XXYYM
        table.insert(cards, table.remove(cards, 3))
    elseif d2 == 0 and d4 == 0 then
        -- MXXYY -> XXYYM
        table.insert(cards, table.remove(cards, 1))
    elseif d1 == 0 then
        -- XXABC
    elseif d2 == 0 then
        -- AXXBC -> XXABC
        table.insert(cards, 3, table.remove(cards, 1))
    elseif d3 == 0 then
        -- ABXXC -> XXABC
        table.insert(cards, 4, table.remove(cards, 1))
        table.insert(cards, 4, table.remove(cards, 1))
    elseif d4 == 0 then
        -- ABCXX -> XXABC
        table.insert(cards, table.remove(cards, 1))
        table.insert(cards, table.remove(cards, 1))
        table.insert(cards, table.remove(cards, 1))
    else
        -- ABCDE
    end

    local c1 = card_color(cards[1])
    local c2 = card_color(cards[2])
    local c3 = card_color(cards[3])
    local c4 = card_color(cards[4])
    local c5 = card_color(cards[5])

    local n1 = card_number(cards[1])
    local n2 = card_number(cards[2])
    local n3 = card_number(cards[3])
    local n4 = card_number(cards[4])
    local n5 = card_number(cards[5])

    local d1 = n1 - n2
    local d2 = n2 - n3
    local d3 = n3 - n4
    local d4 = n4 - n5

    local score = (n1 << 16) | (n2 << 12) | (n3 << 8) | (n4 << 4) | n5
    local is_flush = c1 == c2 and c2 == c3 and c3 == c4 and c4 == c5
    local is_straight = d1 == 1 and d2 == 1 and d3 == 1 and d4 == 1

    if n1 == 14 and n2 == 5 and n3 == 4 and n4 == 3 and n5 == 2 then
        is_straight = true -- A5432
        score = (1 << 16) | (n2 << 12) | (n3 << 8) | (n4 << 4) | n5
    end

    local rank = 0
    if is_flush and is_straight then
        if n5 == 10 then
            rank = texas_rank.ROYAL_FLUSH
        else
            rank = texas_rank.STRAIGHT_FLUSH
        end
    elseif d1 == 0 and d2 == 0 and d3 == 0 then
        rank = texas_rank.FOUR
    elseif d1 == 0 and d2 == 0 and d4 == 0 then
        rank = texas_rank.FULLHOUSE
    elseif is_flush then
        rank = texas_rank.FLUSH
    elseif is_straight then
        rank = texas_rank.STRAIGHT
    elseif d1 == 0 and d2 == 0 then
        rank = texas_rank.THREE
    elseif d1 == 0 and d3 == 0 then
        rank = texas_rank.TWO_PAIR
    elseif d1 == 0 then
        rank = texas_rank.ONE_PAIR
    else
        rank = texas_rank.HIGH_CARD
    end
    
    return (rank << 20) | score
end
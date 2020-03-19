judge = import("gamesvr/texas_judge.lua")

function settle_cur_hand( game )
    local hand = game.current
    for seatid, player in pairs(game.seat_table) do
        player.chips = 100 * seatid
    end
end
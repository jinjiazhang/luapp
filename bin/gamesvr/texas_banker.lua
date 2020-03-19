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
        
    end
end
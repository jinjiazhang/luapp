module = "notify"

waittings = waittings or {}

function wait_for_notify( name )
	local co = coroutine.running()
	waittings[name] = co
	return coroutine.yield(co)
end

function on_recv_notify( name, ... )
	if waittings[name] then
		local co = waittings[name]
		waittings[name] = nil
		coroutine.resume(co, ...)
	end
end

function net.cs_kickout_ntf( reason )
	log_warn("cs_kickout_ntf", reason)
	on_recv_notify("kickout", reason)
end

function net.cs_enter_room_ntf( roomid, viewer )
	-- log_debug("cs_enter_room_ntf", roomid, app.tostring(viewer))
	on_recv_notify("enter_room", roomid, viewer)
end

function net.cs_leave_room_ntf( roomid, roleid, reason )
	-- log_debug("cs_leave_room_ntf", roomid, roleid, reason)
	on_recv_notify("leave_room", roomid, roleid, reason)
end

function net.cs_dismiss_room_ntf( roomid, reason )
	-- log_debug("cs_dismiss_room_ntf", roomid, reason)
	on_recv_notify("dismiss_room", roomid, reason)
end

function net.cs_texas_chat_ntf( roomid, roleid, content )
	-- log_debug("cs_texas_chat_ntf", roomid, roleid, content)
	on_recv_notify("texas_chat", roomid, roleid, content)
end

function net.cs_texas_sitdown_ntf( roomid, player )
	-- log_debug("cs_texas_sitdown_ntf", roomid, player)
	on_recv_notify("texas_sitdown", roomid, player)
end

function net.cs_texas_standup_ntf( roomid, seatid )
	-- log_debug("cs_texas_standup_ntf", roomid, seatid)
	on_recv_notify("texas_standup", roomid, seatid)
end

function net.cs_texas_start_ntf( roomid )
	-- log_debug("cs_texas_start_ntf", roomid)
	on_recv_notify("texas_start", roomid)
end

function net.cs_texas_hand_ntf( roomid, hand )
	-- log_debug("cs_texas_hand_ntf", roomid, hand)
	on_recv_notify("texas_hand", roomid, hand)
end

function net.cs_texas_round_ntf( roomid, hand_idx, round )
	-- log_debug("cs_texas_round_ntf", roomid, hand_idx, round)
	on_recv_notify("texas_round", roomid, hand_idx, round)
end

function net.cs_texas_action_ntf( roomid, hand_idx, round_idx, action )
	-- log_debug("cs_texas_action_ntf", roomid, hand_idx, round_idx, action)
	on_recv_notify("texas_action", roomid, hand_idx, round_idx, action)
end

function net.cs_texas_deal_ntf( roomid, hand_idx, seatid, cards )
	-- log_debug("cs_texas_deal_ntf", roomid, hand_idx, seatid, cards)
	on_recv_notify("texas_deal", roomid, hand_idx, seatid, cards)
end

function net.cs_texas_turn_ntf( roomid, hand_idx, seatid )
	-- log_debug("cs_texas_turn_ntf", roomid, hand_idx, seatid)
	on_recv_notify("texas_turn", roomid, hand_idx, seatid)
end
-- 德州玩法
module = "texas"

function on_create_room( room )
	log_info("on_create_room", room.roomid)
end

function on_enter_room( room, roleid )
	log_info("on_enter_room", room.roomid, roleid)
end

function on_leave_room( room, roleid )
	log_info("on_leave_room", room.roomid, roleid)
end

function on_tick_room( room )
	-- log_info("on_tick_room", room.roomid)
end

function env.cs_texas_chat_req( room, roleid, flowid, content )
	if not room.get_viewer(roleid) then
		log_error("cs_texas_chat_req role not exist", roleid)
		return errno.DATA_ERROR
	end
	
	room.broadcast(0, "cs_texas_chat_ntf", 0, room.roomid, roleid, content)
	return errno.SUCCESS
end

function env.cs_texas_sitdown_req( room, roleid, flowid, seatid )
	
end
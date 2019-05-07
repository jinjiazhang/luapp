-- 房间管理器
module = "texas"

function cs_texas_chat_req( room, roleid, flowid, content )
	if not room.get_viewer(roleid) then
		log_error("cs_texas_chat_req role not exist", roleid)
		return errno.DATA_ERROR
	end
	
	room.broadcast(0, "cs_texas_chat_ntf", 0, room.roomid, roleid, content)
	return errno.SUCCESS
end
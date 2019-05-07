-- 消息通知处理
module = "notify"

function net.cs_kickout_ntf( reason )
	log_warn("cs_kickout_ntf", reason)
end

function net.cs_enter_room_ntf( roomid, viewer )
	log_info("cs_enter_room_ntf", roomid, app.tostring(viewer))
end

function net.cs_leave_room_ntf( roomid, roleid, reason )
	log_info("cs_leave_room_ntf", roomid, roleid, reason)
end

function net.cs_dismiss_room_ntf( roomid, reason )
	log_info("cs_dismiss_room_ntf", roomid, reason)
end

function net.cs_texas_chat_ntf( roomid, roleid, content )
	log_info("cs_texas_chat_ntf", roomid, roleid, content)
end
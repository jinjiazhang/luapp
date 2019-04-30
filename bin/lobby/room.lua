-- 房间逻辑

function net.cs_create_room_req( ss, flowid, name, mode, option )
	log_info("cs_create_room_req", ss.roleid, flowid, name, mode, option)
	
end

function net.cs_fetch_room_req( ss, flowid, mode )
	log_info("cs_fetch_room_req", ss.roleid, flowid, mode)
	
end

function net.cs_enter_room_req( ss, flowid, roomid, cipher )
	log_info("cs_enter_room_req", ss.roleid, flowid, roomid, cipher)
	
end

function net.cs_leave_room_req( ss, flowid )
	log_info("cs_enter_room_req", ss.roleid, flowid)
	
end
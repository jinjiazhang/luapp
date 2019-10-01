
function net.cs_fetch_room_req( ss, mode )
	log_info("cs_fetch_room_req", ss.roleid, mode)
	airport.call_listsvr_mode(mode, "ss_fetch_room_req", ss.roleid, mode)
end

function net.ss_fetch_room_rsp( svrid, result, roleid, room_list )
	log_info("ss_fetch_room_rsp", svrid, result, roleid, room_list)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		return
	end

	ss.cs_fetch_room_rsp(result, room_list)
end

function net.cs_create_room_req( ss, name, mode, option )
	log_info("cs_create_room_req", ss.roleid, name, mode, option)
	if rolemgr.is_gaming(ss.role) then
		ss.cs_create_room_rsp(errno.NEED_RETRY)
		rolemgr.leave_room(ss.role, reason_type.CREATE_ROOM)
		return
	end
	
	airport.call_listsvr_mode(mode, "ss_create_room_req", app.svrid(), ss.role, 0, 0, name, mode, option)
end

function net.ss_create_room_rsp( svrid, result, roleid, room )
	log_info("ss_create_room_rsp", svrid, result, roleid, room)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		return
	end

	if result == errno.SUCCESS then
		rolemgr.on_enter_room(ss.role, svrid, room)
	end
	ss.cs_create_room_rsp(result, room)
end

function net.cs_enter_room_req( ss, roomid, cipher )
	log_info("cs_enter_room_req", ss.roleid, roomid, cipher)
	if rolemgr.is_gaming(ss.role) then
		ss.cs_enter_room_rsp(errno.NEED_RETRY)
		rolemgr.leave_room(ss.role, reason_type.ENTER_ROOM)
		return
	end
	
	-- TODO calc mode
	airport.call_listsvr_mode(0, "ss_enter_room_req", app.svrid(), ss.role, roomid, cipher)
end

function net.ss_enter_room_rsp( svrid, result, roleid, room )
	log_info("ss_enter_room_rsp", svrid, result, roleid, room)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		return
	end

	if result == errno.SUCCESS then
		rolemgr.on_enter_room(ss.role, svrid, room)
	end
	ss.cs_enter_room_rsp(result, room)
end

function net.cs_leave_room_req( ss, roomid )
	log_info("cs_leave_room_req", ss.roleid, roomid)
	if not rolemgr.is_gaming(ss.role) then
		ss.cs_leave_room_rsp(errno.SUCCESS)
		return
	end

	local roleid = ss.role.roleid
	local gaming = ss.role.gaming
	assert(roomid == gaming.roomid)
	airport.call_roomsvr(gaming.rsvrid, "ss_leave_room_req", roleid, gaming.roomid, reason_type.LEAVE_ROOM)
end

function net.ss_leave_room_rsp( svrid, result, roleid, roomid, reason )
	log_info("ss_leave_room_rsp", svrid, result, roleid, roomid, reason)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		return
	end

	rolemgr.on_leave_room(ss.role, roomid, reason)
	if reason == reason_type.LEAVE_ROOM then
		ss.cs_leave_room_rsp(result, roomid)
	end
end

function net.cs_dismiss_room_req( ss, roomid )
	log_info("cs_dismiss_room_req", ss.roleid, roomid)
	if not rolemgr.is_gaming(ss.role) then
		ss.cs_dismiss_room_rsp(errno.SUCCESS)
		return
	end

	local roleid = ss.role.roleid
	local gaming = ss.role.gaming
	assert(roomid == gaming.roomid)
	airport.call_roomsvr(gaming.rsvrid, "ss_dismiss_room_req", roleid, gaming.roomid, reason_type.DISMISS_ROOM)
end

function net.ss_dismiss_room_rsp( svrid, result, roleid, roomid, reason )
	log_info("ss_dismiss_room_rsp", svrid, result, roleid, roomid, reason)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		return
	end

	if result == errno.SUCCESS then
		rolemgr.on_room_dismiss(ss.role, roomid, reason)
	end
	ss.cs_dismiss_room_rsp(result, roomid)
end

function net.cs_dismiss_room_ntf( ss, roomid, reason )
	log_info("cs_dismiss_room_ntf", ss.roleid, roomid, reason)
	rolemgr.on_room_dismiss(ss.role, roomid, reason)
	ss.cs_dismiss_room_ntf(roomid, reason)
end

function net.cs_game_operate_req( ss, req_proto, ... )
	log_info("cs_game_operate_req", ss.roleid, req_proto, ...)
	local rsp_proto = string.gsub(req_proto, "_req", "_rsp")
	if not rolemgr.is_gaming(ss.role) then
		ss[rsp_proto](errno.DATA_ERROR)
		return
	end

	local roleid = ss.role.roleid
	local gaming = ss.role.gaming
	airport.call_roomsvr(gaming.rsvrid, "ss_game_operate_req", roleid, gaming.roomid, req_proto, ...)
end

function net.ss_game_operate_rsp( svrid, result, roleid, rsp_proto, ... )
	log_info("ss_game_operate_rsp", svrid, result, roleid, rsp_proto, ...)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		return
	end

	ss[rsp_proto](...)
end
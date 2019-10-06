
function net.cs_fetch_room_req( ss, mode )
	log_debug("cs_fetch_room_req", ss.roleid, mode)
	airport.call_listsvr_hash(mode, "ss_fetch_room_req", ss.roleid, mode)
end

function net.ss_fetch_room_rsp( svrid, result, roleid, room_list )
	log_debug("ss_fetch_room_rsp", svrid, result, roleid, room_list)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		log_warn("ss_fetch_room_rsp ss not exist", roleid)
		return
	end

	ss.cs_fetch_room_rsp(result, room_list)
end

function net.cs_create_room_req( ss, name, mode, option )
	log_debug("cs_create_room_req", ss.roleid, name, mode, option)
	if rolemgr.is_gaming(ss.role) then
		log_warn("cs_create_room_req room exist", roleid)
		ss.cs_create_room_rsp(errno.NEED_RETRY)
		rolemgr.leave_room(ss.role, reason_type.CREATE_ROOM)
		return
	end
	
	airport.call_listsvr_hash(mode, "ss_assign_room_req", ss.roleid, name, mode, option)
end

function net.ss_assign_room_rsp( svrid, result, roleid, name, mode, option, roomid, cipher, rsvrid)
	log_debug("ss_assign_room_rsp", svrid, roleid, name, mode, option, roomid, cipher, rsvrid)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		log_warn("ss_assign_room_rsp ss not exist", roleid)
		return
	end

	if result ~= errno.SUCCESS then
		ss.cs_create_room_rsp(result)
		return
	end

	airport.call_roomsvr(rsvrid, "ss_create_room_req", ss.role, roomid, cipher, name, mode, option)
end

function net.ss_create_room_rsp( svrid, result, roleid, room )
	log_debug("ss_create_room_rsp", svrid, result, roleid, room)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		log_warn("ss_create_room_rsp ss not exist", roleid)
		return
	end

	if result == errno.SUCCESS then
		rolemgr.on_enter_room(ss.role, svrid, room)
	end
	ss.cs_create_room_rsp(result, room)
end

function net.cs_enter_room_req( ss, roomid, cipher )
	log_debug("cs_enter_room_req", ss.roleid, roomid, cipher)
	if rolemgr.is_gaming(ss.role) then
		log_warn("cs_enter_room_req room exist", roleid)
		ss.cs_enter_room_rsp(errno.NEED_RETRY)
		rolemgr.leave_room(ss.role, reason_type.ENTER_ROOM)
		return
	end
	
	-- TODO set hash mode
	airport.call_listsvr_hash(0, "ss_search_room_req", ss.roleid, roomid, cipher)
end

function net.ss_search_room_rsp( svrid, result, roleid, roomid, cipher, rsvrid )
	log_debug("ss_search_room_rsp", svrid, roleid, roomid, cipher, rsvrid)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		log_warn("ss_assign_room_rsp ss not exist", roleid)
		return
	end

	if result ~= errno.SUCCESS then
		ss.cs_enter_room_rsp(result)
		return
	end

	airport.call_roomsvr(rsvrid, "ss_enter_room_req", ss.role, roomid, cipher)
end

function net.ss_enter_room_rsp( svrid, result, roleid, room )
	log_debug("ss_enter_room_rsp", svrid, result, roleid, room)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		log_warn("ss_enter_room_rsp ss not exist", roleid)
		return
	end

	if result == errno.SUCCESS then
		rolemgr.on_enter_room(ss.role, svrid, room)
	end
	ss.cs_enter_room_rsp(result, room)
end

function net.cs_leave_room_req( ss, roomid )
	log_debug("cs_leave_room_req", ss.roleid, roomid)
	if not rolemgr.is_gaming(ss.role) then
		log_warn("cs_leave_room_req not gaming", roleid)
		ss.cs_leave_room_rsp(errno.SUCCESS)
		return
	end

	local roleid = ss.role.roleid
	local gaming = ss.role.gaming
	assert(roomid == gaming.roomid)
	airport.call_roomsvr(gaming.rsvrid, "ss_leave_room_req", roleid, gaming.roomid, reason_type.LEAVE_ROOM)
end

function net.ss_leave_room_rsp( svrid, result, roleid, roomid, reason )
	log_debug("ss_leave_room_rsp", svrid, result, roleid, roomid, reason)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		log_warn("ss_leave_room_rsp ss not exist", roleid)
		return
	end

	rolemgr.on_leave_room(ss.role, roomid, reason)
	if reason == reason_type.LEAVE_ROOM then
		ss.cs_leave_room_rsp(result, roomid)
	end
end

function net.cs_dismiss_room_req( ss, roomid )
	log_debug("cs_dismiss_room_req", ss.roleid, roomid)
	if not rolemgr.is_gaming(ss.role) then
		log_warn("cs_dismiss_room_req not gaming", roleid)
		ss.cs_dismiss_room_rsp(errno.SUCCESS)
		return
	end

	local roleid = ss.role.roleid
	local gaming = ss.role.gaming
	assert(roomid == gaming.roomid)
	airport.call_roomsvr(gaming.rsvrid, "ss_dismiss_room_req", roleid, gaming.roomid, reason_type.DISMISS_ROOM)
end

function net.ss_dismiss_room_rsp( svrid, result, roleid, roomid, reason )
	log_debug("ss_dismiss_room_rsp", svrid, result, roleid, roomid, reason)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		log_warn("ss_dismiss_room_rsp ss not exist", roleid)
		return
	end

	if result == errno.SUCCESS then
		rolemgr.on_room_dismiss(ss.role, roomid, reason)
	end
	ss.cs_dismiss_room_rsp(result, roomid)
end

function net.cs_dismiss_room_ntf( ss, roomid, reason )
	log_debug("cs_dismiss_room_ntf", ss.roleid, roomid, reason)
	rolemgr.on_room_dismiss(ss.role, roomid, reason)
	ss.cs_dismiss_room_ntf(roomid, reason)
end
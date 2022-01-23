
function net.cs_fetch_room_req( ss, mode )
	log_debug("cs_fetch_room_req", ss.roleid, mode)
	remote.hashcast_explorer(mode, "ss_fetch_room_req", ss.roleid, mode)
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
		log_warn("cs_create_room_req room exist", ss.roleid)
		ss.cs_create_room_rsp(errno.NEED_RETRY)
		rolemgr.leave_room(ss.role, reason_type.CREATE_ROOM)
		return
	end
	
	remote.hashcast_explorer(mode, "ss_apply_room_req", ss.roleid, name, mode, option)
end

function net.ss_apply_room_rsp( svrid, result, roleid, name, mode, option, roomid, roomkey, gsvrid)
	log_debug("ss_apply_room_rsp", svrid, roleid, name, mode, option, roomid, roomkey, gsvrid)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		log_warn("ss_apply_room_rsp ss not exist", roleid)
		return
	end

	if result ~= errno.SUCCESS then
		ss.cs_create_room_rsp(result)
		return
	end

	remote.call_gamesvr(gsvrid, "ss_create_room_req", ss.role, roomid, roomkey, name, mode, option)
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

function net.cs_enter_room_req( ss, mode, roomid, roomkey )
	log_debug("cs_enter_room_req", ss.roleid, roomid, roomkey)
	if rolemgr.is_gaming(ss.role) then
		log_warn("cs_enter_room_req room exist", ss.roleid)
		ss.cs_enter_room_rsp(errno.NEED_RETRY)
		rolemgr.leave_room(ss.role, reason_type.ENTER_ROOM)
		return
	end
	
	remote.hashcast_explorer(mode, "ss_search_room_req", ss.roleid, roomid, roomkey)
end

function net.ss_search_room_rsp( svrid, result, roleid, roomid, roomkey, gsvrid )
	log_debug("ss_search_room_rsp", svrid, roleid, roomid, roomkey, gsvrid)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		log_warn("ss_apply_room_rsp ss not exist", roleid)
		return
	end

	if result ~= errno.SUCCESS then
		ss.cs_enter_room_rsp(result)
		return
	end

	remote.call_gamesvr(gsvrid, "ss_enter_room_req", ss.role, roomid, roomkey)
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

function net.cs_reenter_room_req( ss, roomid )
	log_debug("cs_reenter_room_req", ss.roleid, roomid)
	if not rolemgr.is_gaming(ss.role) then
		log_warn("cs_reenter_room_req not gaming", ss.roleid)
		ss.cs_reenter_room_rsp(errno.NOT_FOUND)
		return
	end
	
	local roleid = ss.role.roleid
	local gaming = ss.role.gaming
	assert(roomid == gaming.roomid)
	remote.call_gamesvr(gaming.gsvrid, "ss_reenter_room_req", ss.role, gaming.roomid)
end

function net.ss_reenter_room_rsp( svrid, result, roleid, room )
	log_debug("ss_reenter_room_rsp", svrid, result, roleid, room)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		log_warn("ss_reenter_room_rsp ss not exist", roleid)
		return
	end

	if result == errno.SUCCESS then
		rolemgr.on_enter_room(ss.role, svrid, room)
	else
		local roomid = ss.role.gaming.roomid
		rolemgr.on_leave_room(ss.role, roomid, reason_type.REENTER_ROOM)
	end
	ss.cs_reenter_room_rsp(result, room)
end

function net.cs_leave_room_req( ss, roomid )
	log_debug("cs_leave_room_req", ss.roleid, roomid)
	if not rolemgr.is_gaming(ss.role) then
		log_warn("cs_leave_room_req not gaming", ss.roleid)
		ss.cs_leave_room_rsp(errno.SUCCESS)
		return
	end

	local roleid = ss.role.roleid
	local gaming = ss.role.gaming
	assert(roomid == gaming.roomid)
	remote.call_gamesvr(gaming.gsvrid, "ss_leave_room_req", roleid, gaming.roomid, reason_type.LEAVE_ROOM)
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
		log_warn("cs_dismiss_room_req not gaming", ss.roleid)
		ss.cs_dismiss_room_rsp(errno.SUCCESS)
		return
	end

	local roleid = ss.role.roleid
	local gaming = ss.role.gaming
	assert(roomid == gaming.roomid)
	remote.call_gamesvr(gaming.gsvrid, "ss_dismiss_room_req", roleid, gaming.roomid, reason_type.DISMISS_ROOM)
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
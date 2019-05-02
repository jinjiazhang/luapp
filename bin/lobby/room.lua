-- 房间逻辑

function net.cs_fetch_room_req( ss, flowid, mode )
	log_info("cs_fetch_room_req", ss.roleid, flowid, mode)
	airport.call_listsvr(mode, "ss_fetch_room_req", flowid, ss.roleid, mode)
end

function net.ss_fetch_room_rsp( svrid, flowid, result, roleid, room_list )
	log_info("ss_fetch_room_rsp", svrid, flowid, result, roleid, #room_list)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		return
	end

	ss.cs_fetch_room_rsp(flowid, result, room_list)
end

function net.cs_create_room_req( ss, flowid, name, mode, option )
	log_info("cs_create_room_req", ss.roleid, flowid, name, mode, option)
	airport.call_listsvr(mode, "ss_create_room_req", flowid, app.svrid(), ss.role, 0, 0, name, mode, option)
end

function net.ss_create_room_rsp( svrid, flowid, result, roleid, room )
	log_info("ss_create_room_rsp", svrid, flowid, result, roleid, room)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		return
	end

	if result == errno.SUCCESS then
		rolemgr.on_enter_room(ss.role, room)
	end
	ss.cs_create_room_rsp(flowid, result, room)
end

function net.cs_enter_room_req( ss, flowid, roomid, cipher )
	log_info("cs_enter_room_req", ss.roleid, flowid, roomid, cipher)
	
	airport.call_listsvr(mode, "ss_enter_room_req", flowid, app.svrid(), ss.role, roomid, cipher)
end

function net.ss_enter_room_rsp( svrid, flowid, result, roleid, room )
	log_info("ss_enter_room_rsp", svrid, flowid, result, roleid, room)
	local ss = ssmgr.find_by_roleid(roleid)
	if not ss then
		return
	end

	if result == errno.SUCCESS then
		rolemgr.on_enter_room(ss.role, room)
	end
	ss.cs_enter_room_rsp(flowid, result, room)
end

function net.cs_leave_room_req( ss, flowid )
	log_info("cs_leave_room_req", ss.roleid, flowid)
	
end
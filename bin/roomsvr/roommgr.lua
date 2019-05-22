module = "roommgr"
texas = import("roomsvr/texas.lua")

support_mode = config.support_mode
total_role_count = total_role_count or 0
total_room_count = total_room_count or 0
last_report_payload = last_report_payload or 0
roomid_room_table = roomid_room_table or {}

function tick(  )
	tick_room_list()
	report_payload()
end

function find_by_roomid( roomid, cipher )
	return roomid_room_table[roomid]
end

function get_room_meta( mode )
	if mode == game_mode.TEXAS then
		return texas
	else
		log_error("get_room_meta unknow mode", mode)
	end
end

function create_room( lsvrid, roomid, cipher, roleid, name, mode, option )
	local room = proto.create("room_detail")
	room.viewer_table = {}
	room.lsvrid = lsvrid
	room.master = roleid
	
	setmetatable(room, {__index = get_room_meta(mode)})
	room.broadcast = function ( exceptid, proto, ... )
		roommgr.broadcast(room, exceptid, proto, ...)
	end

	room.get_viewer = function ( roleid )
		return room.viewer_table[roleid]
	end

	room.roomid = roomid
	room.cipher = cipher
	room.name = name
	room.status = room_status.WAITING
	room.create_time = app.time()
	room.mode = mode
	room.option = option

	roomid_room_table[roomid] = room
	room:on_create_room()
	return room
end

function enter_room( room, lobbyid, role )
	if room.viewer_table[role.roleid] then
		log_error("roommgr.enter_room role already exist", role.roleid)
		return errno.CONFLICT
	end

	role.lobbyid = lobbyid
	table.insert(room.viewers, role)
	room.viewer_table[role.roleid] = role
	room:on_enter_room(role.roleid)
	return errno.SUCCESS
end

function leave_room( room, roleid )
	if not room.viewer_table[roleid] then
		log_error("roommgr.leave_room role not exist", roleid)
		return errno.DATA_ERROR
	end

	for index, role in ipairs(room.viewers) do
		if roleid == role.roleid then
			table.remove(room.viewers, index)
			break
		end
	end

	room:on_leave_room(roleid)
	room.viewer_table[roleid] = nil
	return errno.SUCCESS
end

function dismiss_room( room, roleid )
	return errno.PRIVILEGE
end

function tick_room_list(  )
	for _, room in pairs(roomid_room_table) do
		room:on_tick_room()
	end
end

function report_payload(  )
	local mstime = app.mstime()
	if mstime - last_report_payload >= 1000 then
		airport.call_listsvr_all("ss_report_payload_req", 0, support_mode, total_role_count, total_room_count)
		last_report_payload = mstime
	end
end

function update_listsvr( room )
	airport.call_listsvr(room.lsvrid, "ss_update_room_req", 0, room)
end

function broadcast( room, exceptid, proto, ... )
	for _, viewer in ipairs(room.viewers) do
		if viewer.roleid ~= exceptid then
			log_info(proto, viewer.roleid, ...)
			airport.call_client(viewer.roleid, proto, ...)
		end
	end
end

function net.ss_update_room_rsp( svrid, flowid, result )
	log_info("ss_update_room_rsp", svrid, flowid, result)
	if result ~= errno.SUCCESS then
		log_error("ss_update_room_rsp result", result)
	end
end

function net.ss_report_payload_rsp( svrid, flowid, result )
	-- log_info("ss_report_payload_req", svrid, flowid, result)
	if result ~= errno.SUCCESS then
		log_error("ss_report_payload_req result", result)
	end
end

function net.ss_create_room_req( svrid, flowid, lobbyid, role, roomid, cipher, name, mode, option )
	log_info("ss_create_room_req", svrid, flowid, lobbyid, role, roomid, cipher, name, mode, option)
	assert(roomid > 0 and cipher > 0)
	local room = create_room(svrid, roomid, cipher, role.roleid, name, mode, option)
	if room == nil then
		airport.call_lobby(lobbyid, "ss_create_room_rsp", flowid, errno.UNKNOWN, role.roleid)
		return
	end

	local result = enter_room(room, lobbyid, role)
	if result ~= errno.SUCCESS then
		airport.call_lobby(lobbyid, "ss_create_room_rsp", flowid, result, role.roleid)
		return
	end

	update_listsvr(room)
	airport.call_lobby(lobbyid, "ss_create_room_rsp", flowid, errno.SUCCESS, role.roleid, room)
end

function net.ss_enter_room_req( svrid, flowid, lobbyid, role, roomid, cipher )
	log_info("ss_enter_room_req", svrid, flowid, lobbyid, role, roomid, cipher)
	local room = find_by_roomid(roomid)
	if not room then
		airport.call_lobby(lobbyid, "ss_enter_room_rsp", flowid, errno.NOT_FOUND, role.roleid)
		return
	end

	local result = enter_room(room, lobbyid, role)
	if result ~= errno.SUCCESS then
		airport.call_lobby(lobbyid, "ss_enter_room_rsp", flowid, result, role.roleid)
		return
	end

	local viewer = room.viewer_table[role.roleid]
	room.broadcast(role.roleid, "cs_enter_room_ntf", 0, room.roomid, viewer)
	airport.call_lobby(lobbyid, "ss_enter_room_rsp", flowid, errno.SUCCESS, role.roleid, room)
end

function net.ss_leave_room_req( svrid, flowid, roleid, roomid, reason )
	log_info("ss_leave_room_req", svrid, flowid, roleid, roomid, reason)
	local room = find_by_roomid(roomid)
	if not room then
		airport.call_lobby(svrid, "ss_leave_room_rsp", flowid, errno.NOT_FOUND, roleid, roomid, reason)
		return
	end

	local result = leave_room(room, roleid, reason)
	if result ~= errno.SUCCESS then
		airport.call_lobby(svrid, "ss_leave_room_rsp", flowid, result, roleid, roomid, reason)
		return
	end

	room.broadcast(roleid, "cs_leave_room_ntf", 0, room.roomid, roleid, reason)
	airport.call_lobby(svrid, "ss_leave_room_rsp", flowid, errno.SUCCESS, roleid, roomid, reason)
end

function net.ss_dismiss_room_req( svrid, flowid, roleid, roomid, reason )
	log_info("ss_dismiss_room_req", svrid, flowid, roleid, roomid, reason)
	local room = find_by_roomid(roomid)
	if not room then
		airport.call_lobby(svrid, "ss_dismiss_room_rsp", flowid, errno.NOT_FOUND, roleid, roomid, reason)
		return
	end

	local result = dismiss_room(room, roleid)
	if result ~= errno.SUCCESS then
		airport.call_lobby(svrid, "ss_dismiss_room_rsp", flowid, result, roleid, roomid, reason)
		return
	end

	room.broadcast(roleid, "cs_dismiss_room_ntf", 0, room.roomid, reason)
	airport.call_lobby(svrid, "ss_dismiss_room_rsp", flowid, errno.SUCCESS, roleid, roomid, reason)
end

function net.ss_game_operate_req( svrid, flowid, roleid, roomid, req_proto, ... )
	log_info("ss_game_operate_req", svrid, flowid, roleid, roomid, req_proto, ...)
	local rsp_proto = string.gsub(req_proto, "_req", "_rsp")
	local room = find_by_roomid(roomid)
	if not room then
		airport.call_lobby(svrid, "ss_game_operate_rsp", flowid, errno.NOT_FOUND, roleid, rsp_proto, flowid, errno.NOT_FOUND)
		return
	end

	if not room[req_proto] then
		airport.call_lobby(svrid, "ss_game_operate_rsp", flowid, errno.UNKNOWN, roleid, rsp_proto, flowid, errno.UNKNOWN)
		return
	end

	local rets = { room[req_proto](room, roleid, ...) }
	local result = rets[1] or errno.FAILURE
	airport.call_lobby(svrid, "ss_game_operate_rsp", flowid, result, roleid, rsp_proto, flowid, table.unpack(rets))
end
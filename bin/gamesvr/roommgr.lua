module = "roommgr"
texas = import("gamesvr/texas.lua")

support_mode = config.support_mode
total_role_count = total_role_count or 0
total_room_count = total_room_count or 0
last_report_payload = last_report_payload or 0
roomid_room_table = roomid_room_table or {}
roleid_room_table = roleid_room_table or {}

function tick(  )
	tick_room_list()
	report_payload()
end

function find_by_roomid( roomid )
	return roomid_room_table[roomid]
end

function find_by_roleid( roleid )
	return roleid_room_table[roleid]
end

function get_room_meta( mode )
	if mode == game_mode.TEXAS then
		return texas
	else
		log_error("get_room_meta unknow mode", mode)
	end
end

function create_room( roomid, roomkey, roleid, name, mode, option )
	local room = proto.create("room_detail")
	room.viewer_table = {}
	room.master = roleid
	
	setmetatable(room, {__index = get_room_meta(mode)})
	room.broadcast = function ( exceptid, proto, ... )
		roommgr.broadcast(room, exceptid, proto, ...)
	end

	room.get_viewer = function ( roleid )
		return room.viewer_table[roleid]
	end

	room.roomid = roomid
	room.roomkey = roomkey
	room.name = name
	room.status = room_status.WAITING
	room.create_time = app.time()
	room.mode = mode
	room.option = option

	roomid_room_table[roomid] = room
	room:on_create_room()
	return room
end

function enter_room( room, role, reason )
	if room.viewer_table[role.roleid] then
		log_error("roommgr.enter_room role already exist", role.roleid)
		return errno.CONFLICT
	end

	roleid_room_table[role.roleid] = room
	airport.reg_role(service.GAMESVR, role.roleid)

	table.insert(room.viewers, role)
	room.viewer_table[role.roleid] = role
	room:on_enter_room(role.roleid, reason)
	return errno.SUCCESS
end

function leave_room( room, roleid, reason )
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

	room:on_leave_room(roleid, reason)
	room.viewer_table[roleid] = nil
	roleid_room_table[roleid] = nil
	airport.unreg_role(service.GAMESVR, roleid)
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
		airport.call_roomsvr_all("ss_report_payload_req", support_mode, total_role_count, total_room_count)
		last_report_payload = mstime
	end
end

function refresh_roomsvr( roomid, room )
	airport.call_roomsvr_hash(room.mode, "ss_refresh_room_req", roomid, room)
end

function broadcast( room, exceptid, proto, ... )
	for _, viewer in ipairs(room.viewers) do
		if viewer.roleid ~= exceptid then
			log_debug("room.broadcast", room.roomid, proto, viewer.roleid, ...)
			airport.call_client(viewer.roleid, proto, ...)
		end
	end
end

function net.ss_refresh_room_rsp( svrid, result )
	log_debug("ss_refresh_room_rsp", svrid, result)
	if result ~= errno.SUCCESS then
		log_error("ss_refresh_room_rsp result", result)
	end
end

function net.ss_report_payload_rsp( svrid, result )
	-- log_debug("ss_report_payload_req", svrid, result)
	if result ~= errno.SUCCESS then
		log_error("ss_report_payload_req result", result)
	end
end

function net.ss_create_room_req( svrid, role, roomid, roomkey, name, mode, option )
	log_debug("ss_create_room_req", svrid, role, roomid, roomkey, name, mode, option)
	local room = create_room(roomid, roomkey, role.roleid, name, mode, option)
	if room == nil then
		airport.call_lobby(svrid, "ss_create_room_rsp", errno.UNKNOWN, role.roleid)
		return
	end

	local result = enter_room(room, role, reason_type.CREATE_ROOM)
	if result ~= errno.SUCCESS then
		airport.call_lobby(svrid, "ss_create_room_rsp", result, role.roleid)
		return
	end

	refresh_roomsvr(roomid, room)
	airport.call_lobby(svrid, "ss_create_room_rsp", errno.SUCCESS, role.roleid, room)
end

function net.ss_enter_room_req( svrid, role, roomid, roomkey )
	log_debug("ss_enter_room_req", svrid, role, roomid, roomkey)
	local room = find_by_roomid(roomid)
	if not room then
		airport.call_lobby(svrid, "ss_enter_room_rsp", errno.NOT_FOUND, role.roleid)
		return
	end

	local result = enter_room(room, role, reason_type.ENTER_ROOM)
	if result ~= errno.SUCCESS then
		airport.call_lobby(svrid, "ss_enter_room_rsp", result, role.roleid)
		return
	end

	refresh_roomsvr(roomid, room)
	local viewer = room.viewer_table[role.roleid]
	room.broadcast(role.roleid, "cs_enter_room_ntf", room.roomid, viewer)
	airport.call_lobby(svrid, "ss_enter_room_rsp", errno.SUCCESS, role.roleid, room)
end

function net.ss_reenter_room_req( svrid, role, roomid, roomkey )
	log_debug("ss_reenter_room_req", svrid, role, roomid, roomkey)
	local room = find_by_roomid(roomid)
	if not room then
		airport.call_lobby(svrid, "ss_reenter_room_rsp", errno.NOT_FOUND, role.roleid)
		return
	end

	local result = enter_room(room, role, reason_type.REENTER_ROOM)
	if result ~= errno.SUCCESS then
		airport.call_lobby(svrid, "ss_reenter_room_rsp", result, role.roleid)
		return
	end

	refresh_roomsvr(roomid, room)
	local viewer = room.viewer_table[role.roleid]
	room.broadcast(role.roleid, "cs_enter_room_ntf", room.roomid, viewer)
	airport.call_lobby(svrid, "ss_reenter_room_rsp", errno.SUCCESS, role.roleid, room)
end

function net.ss_leave_room_req( svrid, roleid, roomid, reason )
	log_debug("ss_leave_room_req", svrid, roleid, roomid, reason)
	local room = find_by_roomid(roomid)
	if not room then
		airport.call_lobby(svrid, "ss_leave_room_rsp", errno.NOT_FOUND, roleid, roomid, reason)
		return
	end

	local result = leave_room(room, roleid, reason)
	if result ~= errno.SUCCESS then
		airport.call_lobby(svrid, "ss_leave_room_rsp", result, roleid, roomid, reason)
		return
	end

	refresh_roomsvr(roomid, room)
	room.broadcast(roleid, "cs_leave_room_ntf", room.roomid, roleid, reason)
	airport.call_lobby(svrid, "ss_leave_room_rsp", errno.SUCCESS, roleid, roomid, reason)
end

function net.ss_dismiss_room_req( svrid, roleid, roomid, reason )
	log_debug("ss_dismiss_room_req", svrid, roleid, roomid, reason)
	local room = find_by_roomid(roomid)
	if not room then
		airport.call_lobby(svrid, "ss_dismiss_room_rsp", errno.NOT_FOUND, roleid, roomid, reason)
		return
	end

	local result = dismiss_room(room, roleid)
	if result ~= errno.SUCCESS then
		airport.call_lobby(svrid, "ss_dismiss_room_rsp", result, roleid, roomid, reason)
		return
	end

	refresh_roomsvr(roomid, room)
	room.broadcast(roleid, "cs_dismiss_room_ntf", room.roomid, reason)
	airport.call_lobby(svrid, "ss_dismiss_room_rsp", errno.SUCCESS, roleid, roomid, reason)
end
-- 房间管理器
module = "roommgr"

roomid_room_table = roomid_room_table or {}

function create_room( roomid, cipher, name, mode, option )
	local room = proto.build("room_detail")
	room.viewer_table = {}
	local basic = room.basic
	basic.roomid = roomid
	basic.cipher = cipher
	basic.name = name
	basic.status = room_status.WAITING
	basic.create_time = app.time()
	basic.mode = mode
	basic.option = option

	roomid_room_table[roomid] = room
	return room
end

function enter_room( room, lobbyid, role )
	if room.viewer_table[role.roleid] then
		log_error("roommgr.enter_room exist role", role.roleid)
		return errno.CONFLICT
	end

	role.lobbyid = lobbyid
	table.insert(room.viewers, role)
	room.viewer_table[role.roleid] = role
	return errno.SUCCESS
end

function leave_room( room, roleid )
	if not room.viewer_table[role.roleid] then
		log_error("roommgr.enter_room exist role", role.roleid)
		return errno.DATA_ERROR
	end

	for index, role in ipairs(room.viewers) do
		if roleid == role.roleid then
			table.remove(room.viewers, index)
			break
		end
	end

	room.viewer_table[roleid] = nil
	return errno.SUCCESS
end

function net.ss_create_room_req( svrid, flowid, lobbyid, role, roomid, cipher, name, mode, option )
	log_info("ss_create_room_req", svrid, flowid, lobbyid, role, roomid, cipher, name, mode, option)
	assert(roomid > 0 and cipher > 0)
	local room = create_room(roomid, cipher, name, mode, option)
	if room == nil then
		airport.call_target(lobbyid, "ss_create_room_rsp", flowid, errno.UNKNOWN, role.roleid)
		return
	end

	local result = enter_room(room, lobbyid, role)
	if result ~= errno.SUCCESS then
		airport.call_target(lobbyid, "ss_create_room_rsp", flowid, result, role.roleid)
		return
	end

	airport.call_target(lobbyid, "ss_create_room_rsp", flowid, errno.SUCCESS, role.roleid, room)
end
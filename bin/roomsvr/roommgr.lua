module = "roommgr"

roomid_room_table = roomid_room_table or {}
roomkey_room_table = roomkey_room_table or {}

function gen_roomkey( mode )
	local rand_num = math.random(1, 99999)
	local roomkey = mode * 100000 + rand_num
	if not roomkey_room_table[roomkey] then
		return roomkey
	end
	return gen_roomkey(mode)
end

function find_by_roomid( roomid )
	return roomid_room_table[roomid]
end

function find_by_roomkey( roomkey )
	return roomkey_room_table[roomkey]
end

function refresh_room( gsvrid, roomid, room )
	room.gsvrid = gsvrid
	roomid_room_table[room.roomid] = room
	roomkey_room_table[room.roomkey] = room
	return errno.SUCCESS
end

function net.ss_fetch_room_req( svrid, roleid, mode )
	log_debug("ss_fetch_room_req", svrid, roleid, mode)
	local room_list = {}
	for roomid, room in pairs(roomid_room_table) do
		if room.mode == mode and room.status ~= room_status.INITING then
			table.insert(room_list, room)
		end
	end

	-- log_debug("ss_fetch_room_rsp", app.tostring(room_list))
	remote.call_lobby(svrid, "ss_fetch_room_rsp", errno.SUCCESS, roleid, room_list)
end

function net.ss_refresh_room_req( svrid, roomid, room )
	log_debug("ss_refresh_room_req", svrid, roomid, room)
	local result = refresh_room(svrid, roomid, room)
	remote.call_gamesvr(svrid, "ss_refresh_room_rsp", result)
end

function net.ss_apply_room_req( svrid, roleid, name, mode, option )
	log_debug("ss_apply_room_req", svrid, roleid, name, mode, option)
	local gsvrid = balance.select_gamesvr(mode, option)
	if gsvrid == 0 then
		remote.call_lobby(svrid, "ss_apply_room_rsp", errno.OVERLOAD, roleid)
		return
	end

	local room = proto.create("room_brief")
	room.roomid = unique.gen_roomid()
	room.roomkey = gen_roomkey(mode)
	room.status = room_status.INITING
	room.name = name
	room.mode = mode
	room.option = option
	refresh_room(gsvrid, room.roomid, room)
	remote.call_lobby(svrid, "ss_apply_room_rsp", errno.SUCCESS, roleid, name, mode, option, room.roomid, room.roomkey, gsvrid)
end

function net.ss_search_room_req( svrid, roleid, roomid, roomkey )
	log_debug("ss_search_room_req", svrid, roleid, roomid, roomkey)
	local room = find_by_roomid(roomid)
	if not room then
		room = find_by_roomkey(roomkey)
	end

	if not room then
		remote.call_lobby(svrid, "ss_search_room_rsp", errno.NOT_FOUND, roleid)
		return
	end

	remote.call_lobby(svrid, "ss_search_room_rsp", errno.SUCCESS, roleid, room.roomid, room.roomkey, room.gsvrid)
end
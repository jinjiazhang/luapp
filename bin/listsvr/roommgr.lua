module = "roommgr"

roomid_room_table = roomid_room_table or {}
cipher_room_table = cipher_room_table or {}

function gen_cipher( mode )
	local rand_num = math.random(1, 99999)
	local cipher = mode * 100000 + rand_num
	if not cipher_room_table[cipher] then
		return cipher
	end
	return gen_cipher(mode)
end

function find_by_roomid( roomid )
	return roomid_room_table[roomid]
end

function find_by_cipher( cipher )
	return cipher_room_table[cipher]
end

function refresh_room( rsvrid, roomid, room )
	room.rsvrid = rsvrid
	roomid_room_table[room.roomid] = room
	cipher_room_table[room.cipher] = room
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
	airport.call_lobby(svrid, "ss_fetch_room_rsp", errno.SUCCESS, roleid, room_list)
end

function net.ss_refresh_room_req( svrid, roomid, room )
	log_debug("ss_refresh_room_req", svrid, roomid, room)
	local result = refresh_room(svrid, roomid, room)
	airport.call_roomsvr(svrid, "ss_refresh_room_rsp", result)
end

function net.ss_assign_room_req( svrid, roleid, name, mode, option )
	log_debug("ss_assign_room_req", svrid, roleid, name, mode, option)
	local rsvrid = assign.assign_room(mode, option)
	if rsvrid == 0 then
		airport.call_lobby(svrid, "ss_assign_room_rsp", errno.OVERLOAD, roleid)
		return
	end

	local room = proto.create("room_brief")
	room.roomid = unique.gen_roomid()
	room.cipher = gen_cipher(mode)
	room.status = room_status.INITING
	room.name = name
	room.mode = mode
	room.option = option
	refresh_room(rsvrid, room.roomid, room)
	airport.call_lobby(svrid, "ss_assign_room_rsp", errno.SUCCESS, roleid, name, mode, option, room.roomid, room.cipher, rsvrid)
end

function net.ss_search_room_req( svrid, roleid, roomid, cipher )
	log_debug("ss_search_room_req", svrid, roleid, roomid, cipher)
	local room = find_by_roomid(roomid)
	if not room then
		room = find_by_cipher(cipher)
	end

	if not room then
		airport.call_lobby(svrid, "ss_search_room_rsp", errno.NOT_FOUND, roleid)
		return
	end

	airport.call_lobby(svrid, "ss_search_room_rsp", errno.SUCCESS, roleid, room.roomid, room.cipher, room.rsvrid)
end
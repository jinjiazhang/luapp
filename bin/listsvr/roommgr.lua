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

function select_roomsvr( mode )
	return 0
end

function update_room( rsvrid, room )
	room.rsvrid = rsvrid
	roomid_room_table[room.roomid] = room
	cipher_room_table[room.cipher] = room
	return errno.SUCCESS
end

function search_room( roomid, cipher )
	return roomid_room_table[roomid] or cipher_room_table[cipher]
end

function net.ss_fetch_room_req( svrid, roleid, mode )
	log_debug("ss_fetch_room_req", svrid, roleid, mode)
	local room_list = {}
	for roomid, room in pairs(roomid_room_table) do
		if room.mode == mode then
			table.insert(room_list, room)
		end
	end

	-- log_debug("ss_fetch_room_rsp", app.tostring(room_list))
	airport.call_lobby(svrid, "ss_fetch_room_rsp", errno.SUCCESS, roleid, room_list)
end

function net.ss_update_room_req( svrid, room )
	log_debug("ss_update_room_req", svrid, room)
	local result = update_room(svrid, room)
	airport.call_roomsvr(svrid, "ss_update_room_rsp", result)
end

function net.ss_create_room_req( svrid, lobbyid, role, roomid, cipher, name, mode, option )
	log_debug("ss_create_room_req", svrid, lobbyid, role, roomid, cipher, name, mode, option)
	assert(roomid == 0 and cipher == 0)
	local rsvrid = assign.assign_room(mode)
	if rsvrid == 0 then
		airport.call_lobby(svrid, "ss_create_room_rsp", errno.OVERLOAD, role.roleid)
		return
	end

	local room = proto.create("room_brief")
	room.roomid = unique.gen_roomid()
	room.cipher = gen_cipher(mode)
	room.status = room_status.INITING
	update_room(rsvrid, room)
	airport.call_roomsvr(rsvrid, "ss_create_room_req", lobbyid, role, room.roomid, room.cipher, name, mode, option)
end

function net.ss_enter_room_req( svrid, lobbyid, role, roomid, cipher )
	log_debug("ss_enter_room_req", svrid, lobbyid, role, roomid, cipher)
	local room = search_room(roomid, cipher)
	if not room then
		airport.call_lobby(svrid, "ss_enter_room_rsp", errno.NOT_FOUND, role.roleid)
		return
	end

	local rsvrid = room.rsvrid
	airport.call_roomsvr(rsvrid, "ss_enter_room_req", lobbyid, role, room.roomid, room.cipher)
end
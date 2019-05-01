-- 房间逻辑

roomid_room_table = roomid_room_table or {}

function net.cs_create_room_req( ss, flowid, name, mode, option )
	log_info("cs_create_room_req", ss.roleid, flowid, name, mode, option)
	local room = {roomid = 1001, cipher = 123456}
	roomid_room_table[room.roomid] = room
	ss.cs_create_room_rsp(flowid, errno.SUCCESS, room)
end

function net.cs_fetch_room_req( ss, flowid, mode )
	log_info("cs_fetch_room_req", ss.roleid, flowid, mode)
	local room_list = {}
	for roomid, room in pairs(roomid_room_table) do
		table.insert(room_list, room)
	end
	ss.cs_fetch_room_rsp(flowid, errno.SUCCESS, room_list)
end

function net.cs_enter_room_req( ss, flowid, roomid, cipher )
	log_info("cs_enter_room_req", ss.roleid, flowid, roomid, cipher)
	local room = roomid_room_table[roomid]
	ss.cs_enter_room_rsp(flowid, errno.SUCCESS, room)
end

function net.cs_leave_room_req( ss, flowid )
	log_info("cs_leave_room_req", ss.roleid, flowid)
	
end
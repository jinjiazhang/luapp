-- 房间管理器
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

function net.ss_create_room_req( svrid, flowid, lobbyid, role, roomid, cipher, name, mode, option )
	log_info("ss_create_room_req", svrid, flowid, lobbyid, role, roomid, cipher, name, mode, option)
	assert(roomid == 0 and cipher == 0)
	roomid = unique.gen_roomid()
	cipher = gen_cipher(mode)
	local rsvrid = select_roomsvr(mode)
	airport.call_roomsvr(rsvrid, "ss_create_room_req", flowid, lobbyid, role, roomid, cipher, name, mode, option)
end
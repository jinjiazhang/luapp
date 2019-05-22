module = "assign"

roomsvr_payload_table = roomsvr_payload_table or {}

function assign_room( mode )
	for rsvrid, payload in pairs(roomsvr_payload_table) do
		if payload.support_mode == mode then
			return rsvrid
		end
	end
	return 0
end

function update_payload( svrid, support_mode, role_count, room_count )
	roomsvr_payload_table[svrid] = {
		support_mode = support_mode,
		role_count = role_count,
		room_count = room_count,
	}
	return errno.SUCCESS
end

function net.ss_report_payload_req( svrid, flowid, support_mode, role_count, room_count )
	-- log_info("ss_report_payload_req", svrid, flowid, support_mode, role_count, room_count)
	local result = update_payload(svrid, support_mode, role_count, room_count)
	airport.call_roomsvr(svrid, "ss_report_payload_rsp", flowid, result)
end

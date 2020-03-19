module = "assign"

gamesvr_payload_table = gamesvr_payload_table or {}

function assign_gamesvr( mode, option )
	for gsvrid, payload in pairs(gamesvr_payload_table) do
		if payload.support_mode == mode then
			return gsvrid
		end
	end
	return 0
end

function update_payload( svrid, support_mode, role_count, room_count )
	gamesvr_payload_table[svrid] = {
		support_mode = support_mode,
		role_count = role_count,
		room_count = room_count,
	}
	return errno.SUCCESS
end

function net.ss_report_payload_req( svrid, support_mode, role_count, room_count )
	-- log_info("ss_report_payload_req", svrid, support_mode, role_count, room_count)
	local result = update_payload(svrid, support_mode, role_count, room_count)
	airport.call_gamesvr(svrid, "ss_report_payload_rsp", result)
end

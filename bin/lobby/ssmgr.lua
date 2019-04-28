-- 会话管理器
module = "ssmgr"
number_session_table = number_session_table or {}
openid_session_table = number_session_table or {}
roleid_session_table = number_session_table or {}

function find_by_number( number )
	return number_session_table[number]
end

function find_by_openid( openid )
	return openid_session_table[openid]
end

function find_by_roleid( roleid )
	return roleid_session_table[roleid]
end

function bind_account( number, account )
	
end

function bind_role( number, role )
	
end

function on_start( number )
	local ss = { number = number }
	number_session_table[number] = ss
end

function on_stop( number )
	-- TODO save role
	-- TODO logout
	number_session_table[number] = nil
end

function on_call( number, proto, ... )
	local ss = find_by_number(number)
	if not ss then
		log_error("ssmgr.on_call session not found", number)
		return
	end

	local proc_func = net[proto]
	if not proc_func then
		log_error("ssmgr.on_call proc_func not found", number)
		return
	end

	proc_func(ss, ...)
end
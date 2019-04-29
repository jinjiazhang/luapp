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

function bind_account( ss, account )
	ss.account = account
	ss.openid = account.openid
	openid_session_table[ss.openid] = ss
end

function bind_role( ss, role )
	ss.role = role
	ss.roleid = role.roleid
	roleid_session_table[ss.roleid] = ss
end

function __index_ss( ss, key )
	if proto.belong(key) then
		ss[key] = function ( ... )
			net.call(ss.number, key, ...)
		end
		return ss[key]
	end
end

function on_start( number )
	local ss = { number = number }
	setmetatable(ss, {__index = __index_ss})
	number_session_table[number] = ss
end

function on_stop( number )
	local ss = find_by_number(number)
	if not ss then
		return
	end

	if ss.roleid then
		roleid_session_table[ss.roleid] = nil
	end

	if ss.openid then
		openid_session_table[ss.openid] = nil
	end	

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
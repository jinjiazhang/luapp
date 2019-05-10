-- 会话管理器
module = "ssmgr"
number_session_table = number_session_table or {}
openid_session_table = number_session_table or {}
roleid_session_table = number_session_table or {}
kickout_session_table = kickout_session_table or {}

KICKOUT_DELAY_CLOSE = 1000    	-- 踢号延迟关闭连接时间
KICKOUT_TICK_MAX_NUM = 10		-- 每次tick最多踢多少个连接

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
	rolemgr.on_login(ss.role)
end

function kickout( ss, reason )
	ss.cs_kickout_ntf(0, reason)
	kickout_session_table[ss.number] = app.mstime()
end

function __index_ss( ss, key )
	if proto.belong(key) then
		ss[key] = function ( ... )
			net.call(ss.number, key, ...)
		end
		return ss[key]
	end
end

function tick(  )
	tick_kickout()
end

function tick_kickout(  )
	local count = 0
	local mstime = app.mstime()
	for number, record in pairs(kickout_session_table) do
		if mstime - record >= KICKOUT_DELAY_CLOSE then
			log_info("ssmgr.real_kickout", number, record)
			server.close_conn(number)

			count = count + 1
			if count >= KICKOUT_TICK_MAX_NUM then
				break
			end
		end
	end
end

function on_start( number )
	log_info("ssmgr.on_start", number)
	local ss = { number = number }
	setmetatable(ss, {__index = __index_ss})
	number_session_table[number] = ss
end

function on_stop( number )
	log_info("ssmgr.on_stop", number)
	local ss = find_by_number(number)
	if not ss then
		return
	end

	if ss.roleid then
		rolemgr.on_logout(ss.role)
		dbagent.ss_save_role_req(0, ss.openid, ss.role)
		roleid_session_table[ss.roleid] = nil
		ss.role = nil
		ss.roleid = nil
	end

	if ss.openid then
		dbagent.ss_logout_req(0, ss.openid, app.svrid())
		openid_session_table[ss.openid] = nil
		ss.account = nil
		ss.openid = nil
	end

	number_session_table[number] = nil
	kickout_session_table[number] = nil
end

proto_handle_table = {
	cs_texas_chat_req = "cs_game_operate_req",
	cs_texas_sitdown_req = "cs_game_operate_req",
	cs_texas_standup_req = "cs_game_operate_req",
	cs_texas_start_req = "cs_game_operate_req",
}

function on_call( number, proto, ... )
	local ss = find_by_number(number)
	if not ss then
		log_error("ssmgr.on_call session not found", number)
		return
	end

	if proto_handle_table[proto] then
		local handle = proto_handle_table[proto]
		net[handle](ss, proto, ...)
		return
	end

	local proc_func = net[proto]
	if not proc_func then
		log_error("ssmgr.on_call proc_func not found", number)
		return
	end

	proc_func(ss, ...)
end
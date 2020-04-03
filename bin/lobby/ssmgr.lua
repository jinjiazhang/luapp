module = "ssmgr"

connid_session_table = connid_session_table or {}
openid_session_table = openid_session_table or {}
roleid_session_table = roleid_session_table or {}
kickout_session_table = kickout_session_table or {}

KICKOUT_DELAY_CLOSE = 1000    	-- 踢号延迟关闭连接时间
KICKOUT_TICK_MAX_NUM = 10		-- 每次tick最多踢多少个连接

function find_by_connid( connid )
	return connid_session_table[connid]
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
	log_info("ssmgr.kickout", ss.connid, ss.openid, reason)
	ss.cs_kickout_ntf(reason)
	kickout_session_table[ss.connid] = app.mstime()
end

function __index_ss( ss, key )
	if proto.exist(key) then
		ss[key] = function ( ... )
			server.call_client(ss.connid, key, ...)
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
	for connid, record in pairs(kickout_session_table) do
		if mstime - record >= KICKOUT_DELAY_CLOSE then
			log_info("ssmgr.real_kickout", connid, record)
			server.close_conn(connid)

			count = count + 1
			if count >= KICKOUT_TICK_MAX_NUM then
				break
			end
		end
	end
end

function on_start( connid )
	log_info("ssmgr.on_start", connid)
	local ss = { connid = connid }
	setmetatable(ss, {__index = __index_ss})
	connid_session_table[connid] = ss
end

function on_stop( connid )
	log_info("ssmgr.on_stop", connid)
	local ss = find_by_connid(connid)
	if not ss then
		return
	end

	if ss.roleid then
		rolemgr.on_logout(ss.role)
		dbagent.ss_save_role_req(ss.openid, ss.roleid, ss.role)
		roleid_session_table[ss.roleid] = nil
		ss.role = nil
		ss.roleid = nil
	end

	if ss.openid then
		dbagent.ss_logout_req(ss.openid, app.svrid())
		openid_session_table[ss.openid] = nil
		ss.account = nil
		ss.openid = nil
	end

	connid_session_table[connid] = nil
	kickout_session_table[connid] = nil
end

proto_transmit_table = {
	["cs_texas_chat_req"] = service.GAMESVR,
	["cs_texas_sitdown_req"] = service.GAMESVR,
	["cs_texas_standup_req"] = service.GAMESVR,
	["cs_texas_start_req"] = service.GAMESVR,
	["cs_texas_action_req"] = service.GAMESVR,
}

function on_call( connid, proto, ... )
	local ss = find_by_connid(connid)
	if not ss then
		log_error("ssmgr.on_call session not found", connid)
		return
	end

	if ss.roleid and proto_transmit_table[proto] then
		local group = proto_transmit_table[proto]
		airport.call_transmit(group, ss.roleid, proto, ...)
		return
	end

	local proc_func = net[proto]
	if not proc_func then
		log_error("ssmgr.on_call proc_func not found", connid)
		return
	end

	proc_func(ss, ...)
end
-- 会话管理器
module = "rolemgr"
openid_role_table = number_role_table or {}
roleid_role_table = number_role_table or {}

SAVE_ONLINE_INTERVAL = 25    	-- 保存在线状态的时间间隔
ONLINE_TICK_MAX_NUM = 10		-- 每次tick最多保存多少个在线状态

function find_by_openid( openid )
	return openid_role_table[openid]
end

function find_by_roleid( roleid )
	return roleid_role_table[roleid]
end

function on_login( role )
	log_info("rolemgr.on_login", role.roleid, role.name)
	role.online = app.time()
	role.save_online = app.time()
	roleid_role_table[role.roleid] = role
	openid_role_table[role.openid] = role
end

function on_logout( role )
	log_info("rolemgr.on_logout", role.roleid, role.name)
	role.offline = app.time()
	role.save_online = nil
	roleid_role_table[role.roleid] = nil
	openid_role_table[role.openid] = nil
end

function on_enter_room( role, rsvrid, room )
	-- body
end

function on_leave_room( role )
	-- body
end

function tick(  )
	tick_online()
end

function tick_online(  )
	local count = 0
	local time = app.time()
	for roleid, role in pairs(roleid_role_table) do
		if time - role.save_online >= SAVE_ONLINE_INTERVAL then
			log_info("rolemgr.save_online", role.openid, role.roleid)
			dbagent.ss_online_req(0, role.openid, app.svrid())
			role.save_online = time

			count = count + 1
			if count >= ONLINE_TICK_MAX_NUM then
				break
			end
		end
	end
end
module = "rolemgr"

openid_role_table = openid_role_table or {}
roleid_role_table = roleid_role_table or {}

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
	airport.reg_role(role.roleid)
	
	role.online = app.time()
	role.save_online = app.time()
	roleid_role_table[role.roleid] = role
	openid_role_table[role.openid] = role
end

function on_logout( role )
	log_info("rolemgr.on_logout", role.roleid, role.name)
	leave_room(role, 0, reason_type.ROLE_OFFLINE)
	airport.unreg_role(role.roleid)

	role.offline = app.time()
	role.save_online = nil
	roleid_role_table[role.roleid] = nil
	openid_role_table[role.openid] = nil
end

function is_gaming( role )
	local gaming = role.gaming
	return gaming.rsvrid > 0 and gaming.roomid > 0
end

function leave_room( role, reason )
	local gaming = role.gaming
	if gaming.rsvrid > 0 and gaming.roomid > 0 then
		airport.call_roomsvr(gaming.rsvrid, "ss_leave_room_req", role.roleid, gaming.roomid, reason)
	end
end

function on_enter_room( role, rsvrid, room )
	role.gaming.roomid = room.roomid
	role.gaming.rsvrid = rsvrid
	role.gaming.mode = room.mode
end

function on_leave_room( role, roomid, reason )
	role.gaming.roomid = 0
	role.gaming.rsvrid = 0
	role.gaming.mode = 0
end

function on_room_dismiss( role, roomid, reason )
	role.gaming.roomid = 0
	role.gaming.rsvrid = 0
	role.gaming.mode = 0
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
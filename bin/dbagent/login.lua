-- 登录逻辑

ONLINE_VALID_DURATION = 28		-- 在线状态有效期
UNIQUE_ROLEID_MINIMUM = 112358	-- 最小的角色id

function gen_unique_roleid( name, retry )
	if retry <= 0 then
		return errno.NEED_RETRY
	end

	local limit = "name='unique_roleid'"
	local code, data = sqlpool.sql_select("tb_global", limit)
	if code < 0 then
		return errno.SERVICE
	end

	if not data then
		data = {
			name = "unique_roleid", 
			value = UNIQUE_ROLEID_MINIMUM, 
			text = tostring(UNIQUE_ROLEID_MINIMUM),
			magic = unique.gen_magic(),
		}
		if sqlpool.sql_insert("tb_global", data) < 0 then
			return gen_unique_roleid(name, retry - 1)
		end
		return errno.SUCCESS, data.value
	else
		limit = string.format("name='unique_roleid' and magic=%s", data.magic)
		data.magic = unique.gen_magic()
		data.value = data.value + 1
		local code = sqlpool.sql_update("tb_global", data, limit)
		if code < 0 then
			return errno.SERVICE
		elseif code == 0 then
			return gen_unique_roleid(name, retry - 1)
		elseif code == 1 then
			return errno.SUCCESS, data.value
		else
			return errno.UNKNOWN
		end
	end
end

function net.ss_login_req(ss, flowid, number, openid, svrid)
	log_info("ss_login_req", ss.number, flowid, number, openid, svrid)
	local limit = string.format("openid='%s'", openid)
	local code, data = sqlpool.sql_select("tb_online", limit)
	if code < 0 then
		ss.ss_login_rsp(flowid, errno.SERVICE, number, openid)
		return
	end

	if data and data.svrid > 0 then 
		if app.time() - data.online < ONLINE_VALID_DURATION then
			ss.ss_login_rsp(flowid, errno.CONFLICT, number, openid, data.svrid)
			return
		end

		-- invalid online recode
		if sqlpool.sql_delete("tb_online", limit) < 0 then
			ss.ss_logout_rsp(flowid, errno.SERVICE, number, openid)
			return
		end
	end

	data = { openid = openid, svrid = svrid, online = app.time() }
	if sqlpool.sql_insert("tb_online", data) < 0 then
		ss.ss_login_rsp(flowid, errno.SERVICE, number, openid)
		return
	end

	local code, account = sqlpool.sql_select("tb_account", limit)
	if code < 0 then
		ss.ss_login_rsp(flowid, errno.SERVICE, number, openid)
		return
	end

	if not account then
		account = { openid = openid, name = "", roleid = 0 }
		if sqlpool.sql_insert("tb_account", account) < 0 then
			ss.ss_login_rsp(flowid, errno.SERVICE, number, openid)
			return
		end
	end

	ss.ss_login_rsp(flowid, errno.SUCCESS, number, openid, svrid, account)
end

function net.ss_logout_req( ss, flowid, openid, svrid )
	log_info("ss_logout_req", ss.number, flowid, openid, svrid)
	local limit = string.format("openid='%s'", openid)
	local code, data = sqlpool.sql_select("tb_online", limit)
	if code < 0 or not data then
		ss.ss_logout_rsp(flowid, errno.SERVICE, openid)
		return
	end

	if svrid ~= data.svrid then
		ss.ss_logout_rsp(flowid, errno.CONFLICT, openid)
		return
	end

	limit = string.format("openid='%s' and svrid=%d", openid, svrid)
	if sqlpool.sql_delete("tb_online", limit) < 0 then
		ss.ss_logout_rsp(flowid, errno.SERVICE, openid)
		return
	end

	ss.ss_logout_rsp(flowid, errno.SUCCESS, openid)
end

function net.ss_online_req( ss, flowid, openid, svrid )
	log_info("ss_online_req", ss.number, flowid, openid, svrid)
	local limit = string.format("openid='%s' and svrid=%d", openid, svrid)
	local data = { openid = openid, svrid = svrid, online = app.time() }
	if sqlpool.sql_update("tb_online", data, limit) < 0 then
		ss.ss_online_rsp(flowid, errno.SERVICE, openid)
		return
	end

	ss.ss_online_rsp(flowid, errno.SUCCESS, openid)
end

function net.ss_create_role_req(ss, flowid, openid, name)
	log_info("ss_create_role_req", ss.number, flowid, openid, name)
	local limit = string.format("openid='%s'", openid)
	local code, account = sqlpool.sql_select("tb_account", limit)
	if code < 0 then
		ss.ss_create_role_rsp(flowid, errno.SERVICE, openid)
		return
	end

	if not account or account.roleid ~= 0 then
		ss.ss_create_role_rsp(flowid, errno.DATA_ERROR, openid)
		return
	end

	local result, roleid = gen_unique_roleid(name, 3)
	if result ~= errno.SUCCESS then
		ss.ss_create_role_rsp(flowid, errno.SERVICE, openid)
	end

	account.roleid = roleid
	account.name = name

	if sqlpool.sql_update("tb_account", account, limit) < 0 then
		ss.ss_create_role_rsp(flowid, errno.SERVICE, openid)
		return
	end

	local role = proto.create("tb_role")
	role.roleid = roleid
	role.openid = openid
	role.name = name
	role.register = app.time()
	role.online = role.register
	role.offline = role.register

	if sqlpool.sql_insert("tb_role", role) < 0 then
		ss.ss_create_role_rsp(flowid, errno.SERVICE, openid)
		return
	end

	ss.ss_create_role_rsp(flowid, errno.SUCCESS, openid, role)
end

function net.ss_load_role_req( ss, flowid, openid, roleid )
	log_info("ss_load_role_req", ss.number, flowid, openid, roleid)
	local limit = string.format("roleid='%s'", roleid)
	local code, role = sqlpool.sql_select("tb_role", limit)
	if code < 0 then
		ss.ss_load_role_rsp(flowid, errno.SERVICE, openid)
		return
	end

	if not role then
		ss.ss_load_role_rsp(flowid, errno.DATA_ERROR, openid)
		return
	end

	if openid ~= role.openid then
		ss.ss_load_role_rsp(flowid, errno.DATA_ERROR, openid)
		return
	end

	ss.ss_load_role_rsp(flowid, errno.SUCCESS, openid, role)
end

function net.ss_save_role_req( ss, flowid, openid, role )
	log_info("ss_save_role_req", ss.number, flowid, openid, role.roleid)
	local limit = string.format("roleid='%s'", role.roleid)
	if sqlpool.sql_update("tb_role", role, limit) < 0 then
		ss.ss_save_role_rsp(flowid, errno.SERVICE, openid)
		return
	end

	ss.ss_save_role_rsp(flowid, errno.SUCCESS, openid, role.roleid)
end
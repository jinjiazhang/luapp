-- 登录逻辑

function net.ss_login_req(ss, flowid, number, openid, svrid)
	log_info("ss_login_req", ss.number, flowid, number, openid, svrid)
	local limit = string.format("openid='%s'", openid)
	local code, data = sqlpool.sql_select("tb_online", limit)
	if code < 0 then
		ss.ss_login_rsp(flowid, errno.SERVICE, number, openid)
		return
	end

	if data and app.time() - data.online < 28 then
		ss.ss_login_rsp(flowid, errno.CONFLICT, number, openid, data.svrid)
		return
	end

	if not data then
		data = { openid = openid, svrid = svrid, online = app.time() }
		if sqlpool.sql_insert("tb_online", data) < 0 then
			ss.ss_login_rsp(flowid, errno.SERVICE, number, openid)
			return
		end
	else
		-- TODO check magic
		data.online = app.time()
		if sqlpool.sql_update("tb_online", data, limit) < 0 then
			ss.ss_login_rsp(flowid, errno.SERVICE, number, openid)
			return
		end
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

	if svrid ~= data.svrid and app.time() - data.online < 28 then
		ss.ss_logout_rsp(flowid, errno.CONFLICT, openid)
		return
	end

	data.online = 0
	if sqlpool.sql_update("tb_online", data, limit) < 0 then
		ss.ss_logout_rsp(flowid, errno.SERVICE, openid)
		return
	end

	ss.ss_logout_rsp(flowid, errno.SUCCESS, openid)
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

	local roleid = unique.gen_roleid()
	account.roleid = roleid
	account.name = name

	if sqlpool.sql_update("tb_account", account, limit) < 0 then
		ss.ss_create_role_rsp(flowid, errno.SERVICE, openid)
		return
	end

	local role = proto.build("tb_role")
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
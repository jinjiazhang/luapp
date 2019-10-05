
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

function net.ss_login_req(ss, number, openid, svrid)
	log_info("ss_login_req", ss.number, number, openid, svrid)
	local limit = string.format("openid='%s'", openid)
	local ok, data = dbimpl.get_online_info(openid)
	if not ok then
		ss.ss_login_rsp(errno.SERVICE, number, openid)
		return
	end

	if data and data.svrid > 0 then
		if app.time() - data.online < ONLINE_VALID_DURATION then
			ss.ss_login_rsp(errno.CONFLICT, number, openid, data.svrid)
			return
		end

		-- invalid online recode
		if not dbimpl.clean_online_info(openid) then
			ss.ss_logout_rsp(errno.SERVICE, number, openid)
			return
		end
	end

	data = { openid = openid, svrid = svrid, online = app.time() }
	if not dbimpl.set_online_info(openid, data) then
		ss.ss_login_rsp(errno.SERVICE, number, openid)
		return
	end

	local ok, account = dbimpl.load_account_data(openid)
	if not ok then
		ss.ss_login_rsp(errno.SERVICE, number, openid)
		return
	end

	if not account then
		account = { openid = openid, name = "", roleid = 0 }
		if dbimpl.save_account_data(openid, account) then
			ss.ss_login_rsp(errno.SERVICE, number, openid)
			return
		end
	end

	ss.ss_login_rsp(errno.SUCCESS, number, openid, svrid, account)
end

function net.ss_logout_req( ss, openid, svrid )
	log_info("ss_logout_req", ss.number, openid, svrid)
	local ok, data = dbimpl.get_online_info(openid)
	if not ok or not data then
		ss.ss_logout_rsp(errno.SERVICE, openid)
		return
	end

	if svrid ~= data.svrid then
		ss.ss_logout_rsp(errno.CONFLICT, openid)
		return
	end

	if not dbimpl.clean_online_info(openid) then
		ss.ss_logout_rsp(errno.SERVICE, openid)
		return
	end

	ss.ss_logout_rsp(errno.SUCCESS, openid)
end

function net.ss_online_req( ss, openid, svrid )
	log_info("ss_online_req", ss.number, openid, svrid)
	local data = { openid = openid, svrid = svrid, online = app.time() }
	if not dbimpl.set_online_info(openid, data) then
		ss.ss_online_rsp(errno.SERVICE, openid)
		return
	end

	ss.ss_online_rsp(errno.SUCCESS, openid)
end

function net.ss_create_role_req(ss, openid, name)
	log_info("ss_create_role_req", ss.number, openid, name)
	local ok, account = dbimpl.load_account_data(openid)
	if not ok then
		ss.ss_create_role_rsp(errno.SERVICE, openid)
		return
	end

	if not account or account.roleid ~= 0 then
		ss.ss_create_role_rsp(errno.DATA_ERROR, openid)
		return
	end

	local ok, roleid = dbimpl.gen_unique_roleid()
	if not ok then
		ss.ss_create_role_rsp(errno.SERVICE, openid)
		return
	end

	account.roleid = roleid
	account.name = name

	if not dbimpl.save_account_data(openid, account) then
		ss.ss_create_role_rsp(errno.SERVICE, openid)
		return
	end

	local role = proto.create("tb_role")
	role.roleid = roleid
	role.openid = openid
	role.name = name
	role.register = app.time()
	role.online = role.register
	role.offline = role.register

	if not dbimpl.save_role_data(roleid, role) then
		ss.ss_create_role_rsp(errno.SERVICE, openid)
		return
	end

	ss.ss_create_role_rsp(errno.SUCCESS, openid, role)
end

function net.ss_load_role_req( ss, openid, roleid )
	log_info("ss_load_role_req", ss.number, openid, roleid)
	local ok, role = dbimpl.load_role_data(roleid)
	if not ok then
		ss.ss_load_role_rsp(errno.SERVICE, openid)
		return
	end

	if not role then
		ss.ss_load_role_rsp(errno.DATA_ERROR, openid)
		return
	end

	if openid ~= role.openid then
		ss.ss_load_role_rsp(errno.DATA_ERROR, openid)
		return
	end

	ss.ss_load_role_rsp(errno.SUCCESS, openid, role)
end

function net.ss_save_role_req( ss, openid, role )
	log_info("ss_save_role_req", ss.number, openid, role.roleid)
	if not dbimpl.save_role_data(roleid, role) then
		ss.ss_save_role_rsp(errno.SERVICE, openid)
		return
	end

	ss.ss_save_role_rsp(errno.SUCCESS, openid, role.roleid)
end
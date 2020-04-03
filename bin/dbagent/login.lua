ONLINE_VALID_DURATION = 28		-- 在线状态有效期

function net.ss_login_req(ss, connid, openid, svrid)
	log_debug("ss_login_req", ss.number, connid, openid, svrid)
	local ok, data = dbimpl.get_online_info(openid)
	if not ok then
		log_error("ss_login_req get_online_info failed", openid)
		ss.ss_login_rsp(errno.SERVICE, connid, openid)
		return
	end

	if data and data.svrid > 0 then
		if app.time() - data.online < ONLINE_VALID_DURATION then
			log_error("ss_login_req online info exist", openid)
			ss.ss_login_rsp(errno.CONFLICT, connid, openid, data.svrid)
			return
		end

		-- invalid online recode
		if not dbimpl.clean_online_info(openid) then
			log_error("ss_login_req clean_online_info failed", openid)
			ss.ss_logout_rsp(errno.SERVICE, connid, openid)
			return
		end
	end

	data = { openid = openid, svrid = svrid, online = app.time() }
	if not dbimpl.insert_online_info(data) then
		log_error("ss_login_req insert_online_info failed", openid)
		ss.ss_login_rsp(errno.SERVICE, connid, openid)
		return
	end

	local ok, account = dbimpl.load_account_data(openid)
	if not ok then
		log_error("ss_login_req load_account_data failed", openid)
		ss.ss_login_rsp(errno.SERVICE, connid, openid)
		return
	end

	if not account then
		account = { openid = openid, name = "", roleid = 0 }
		if not dbimpl.create_new_account(account) then
			log_error("ss_login_req create_new_account failed", openid)
			ss.ss_login_rsp(errno.SERVICE, connid, openid)
			return
		end
	end

	ss.ss_login_rsp(errno.SUCCESS, connid, openid, svrid, account)
end

function net.ss_logout_req( ss, openid, svrid )
	log_debug("ss_logout_req", ss.number, openid, svrid)
	local ok, data = dbimpl.get_online_info(openid)
	if not ok or not data then
		log_error("ss_logout_req get_online_info failed", openid)
		ss.ss_logout_rsp(errno.SERVICE, openid)
		return
	end

	if svrid ~= data.svrid then
		log_error("ss_logout_req online svrid diff", openid, svrid, data.svrid)
		ss.ss_logout_rsp(errno.CONFLICT, openid)
		return
	end

	if not dbimpl.clean_online_info(openid) then
		log_error("ss_logout_req clean_online_info failed", openid)
		ss.ss_logout_rsp(errno.SERVICE, openid)
		return
	end

	ss.ss_logout_rsp(errno.SUCCESS, openid)
end

function net.ss_set_online_req( ss, openid, svrid )
	log_debug("ss_set_online_req", ss.number, openid, svrid)
	local data = { openid = openid, svrid = svrid, online = app.time() }
	if not dbimpl.set_online_info(openid, data) then
		log_error("ss_set_online_req set_online_info failed", openid)
		ss.ss_online_rsp(errno.SERVICE, openid)
		return
	end

	ss.ss_set_online_rsp(errno.SUCCESS, openid)
end

function net.ss_create_role_req(ss, openid, name)
	log_debug("ss_create_role_req", ss.number, openid, name)
	local ok, account = dbimpl.load_account_data(openid)
	if not ok or not account then
		log_error("ss_create_role_req load_account_data failed", openid)
		ss.ss_create_role_rsp(errno.SERVICE, openid)
		return
	end

	if account.openid ~= openid or account.roleid ~= 0 then
		log_error("ss_create_role_req account data invalid", openid, account.openid, account.roleid)
		ss.ss_create_role_rsp(errno.DATA_ERROR, openid)
		return
	end

	local ok, roleid = dbimpl.gen_unique_roleid()
	if not ok then
		log_error("ss_create_role_req gen_unique_roleid failed", openid)
		ss.ss_create_role_rsp(errno.SERVICE, openid)
		return
	end

	account.roleid = roleid
	account.name = name

	if not dbimpl.save_account_data(openid, account) then
		log_error("ss_create_role_req save_account_data failed", openid)
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

	if not dbimpl.create_new_role(role) then
		log_error("ss_create_role_req create_new_role failed", openid)
		ss.ss_create_role_rsp(errno.SERVICE, openid)
		return
	end

	ss.ss_create_role_rsp(errno.SUCCESS, openid, role)
end

function net.ss_load_role_req( ss, openid, roleid )
	log_debug("ss_load_role_req", ss.number, openid, roleid)
	local ok, role = dbimpl.load_role_data(roleid)
	if not ok then
		log_error("ss_load_role_req load_role_data failed", openid, roleid)
		ss.ss_load_role_rsp(errno.SERVICE, openid)
		return
	end

	if not role then
		log_error("ss_load_role_req role data nil", openid, roleid)
		ss.ss_load_role_rsp(errno.DATA_ERROR, openid)
		return
	end

	if openid ~= role.openid or roleid ~= role.roleid then
		log_error("ss_load_role_req role data invalid", openid, roleid)
		ss.ss_load_role_rsp(errno.DATA_ERROR, openid)
		return
	end

	ss.ss_load_role_rsp(errno.SUCCESS, openid, role)
end

function net.ss_save_role_req( ss, openid, roleid, role )
	log_debug("ss_save_role_req", ss.number, openid, roleid)
	if not dbimpl.save_role_data(roleid, role) then
		log_error("ss_save_role_req save_role_data failed", openid, roleid)
		ss.ss_save_role_rsp(errno.SERVICE, openid)
		return
	end

	ss.ss_save_role_rsp(errno.SUCCESS, openid, roleid)
end
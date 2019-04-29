-- 登录逻辑

function net.cs_login_req( ss, flowid, openid, token )
	log_info("cs_login_req", ss.number, flowid, openid, token)
	dbagent.ss_login_req(flowid, ss.number, openid, app.svrid())
end

function net.ss_login_rsp( flowid, result, number, openid, svrid, account )
	log_info("ss_login_rsp", flowid, result, number, openid, svrid, account)
	local ss = ssmgr.find_by_number(number)
	if not ss then
		return
	end

	if result == errno.SUCCESS then
		ssmgr.bind_account(ss, account)	
	elseif result == errno.CONFLICT then
		result = errno.NEED_RETRY
		airport.call_target(svrid, "ss_kickout_ntf", flowid, openid, errno.CONFLICT)
		log_info("notify kickout", svrid, flowid, openid)
	end
	ss.cs_login_rsp(flowid, result, account)
end

function net.ss_logout_rsp( flowid, result, openid )
	log_info("ss_logout_rsp", flowid, result, openid)
	if result ~= errno.SUCCESS then
		log_error("ss_logout_rsp", result, openid)
	end
end

function net.ss_kickout_ntf( svrid, flowid, openid, reason )
	log_info("ss_kickout_ntf", svrid, flowid, openid)
	local ss = ssmgr.find_by_openid(openid)
	if not ss then
		return
	end

	ssmgr.kickout(ss, reason)
end

function net.cs_create_role_req( ss, flowid, name )
	log_info("cs_create_role_req", ss.number, flowid, name)
	dbagent.ss_create_role_req(flowid, ss.openid, name)
end

function net.ss_create_role_rsp( flowid, result, openid, role )
	log_info("ss_create_role_rsp", flowid, flowid, result, openid, role)
	local ss = ssmgr.find_by_openid(openid)
	if not ss then
		return
	end

	if result == errno.SUCCESS then
		ssmgr.bind_role(ss, role)
	end
	ss.cs_create_role_rsp(flowid, result, role)
end

function net.cs_select_role_req( ss, flowid, roleid )
	log_info("cs_select_role_req", ss.number, flowid, roleid)
	dbagent.ss_load_role_req(flowid, ss.openid, roleid)
end

function net.ss_load_role_rsp( flowid, result, openid, role )
	log_info("ss_load_role_rsp", flowid, result, openid, role.roleid)
	local ss = ssmgr.find_by_openid(openid)
	if not ss then
		return
	end

	if result == errno.SUCCESS then
		ssmgr.bind_role(ss, role)
	end
	ss.cs_select_role_rsp(flowid, result, role)
end

function net.ss_save_role_rsp( flowid, result, openid, roleid )
	log_info("ss_save_role_rsp", flowid, result, openid, roleid)
	if result ~= errno.SUCCESS then
		log_error("ss_save_role_rsp", result, openid, roleid)
	end
end
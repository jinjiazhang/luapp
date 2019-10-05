
function net.cs_login_req( ss, openid, token )
	log_debug("cs_login_req", ss.number, openid, token)
	dbagent.ss_login_req(ss.number, openid, app.svrid())
end

function net.ss_login_rsp( result, number, openid, svrid, account )
	log_debug("ss_login_rsp", result, number, openid, svrid, account)
	local ss = ssmgr.find_by_number(number)
	if not ss then
		log_warn("ss_login_rsp ss not exist", openid)
		return
	end

	if result == errno.SUCCESS then
		ssmgr.bind_account(ss, account)	
	elseif result == errno.CONFLICT then
		result = errno.NEED_RETRY
		airport.call_target(svrid, "ss_kickout_ntf", openid, errno.CONFLICT)
		log_info("ss_login_rsp notify kickout", openid, svrid)
	end
	ss.cs_login_rsp(result, account)
end

function net.ss_logout_rsp( result, openid )
	log_debug("ss_logout_rsp", result, openid)
	if result ~= errno.SUCCESS then
		log_error("ss_logout_rsp return failed", result, openid)
	end
end

function net.ss_kickout_ntf( svrid, openid, reason )
	log_debug("ss_kickout_ntf", svrid, openid)
	local ss = ssmgr.find_by_openid(openid)
	if not ss then
		log_warn("ss_kickout_ntf ss not exist", openid)
		return
	end

	ssmgr.kickout(ss, reason)
end

function net.ss_set_online_rsp( result, openid )
	log_debug("ss_set_online_rsp", result, openid)
	if result ~= errno.SUCCESS then
		local ss = ssmgr.find_by_openid(openid)
		if not ss then
			return
		end
		
		log_error("ss_set_online_rsp return failed", result, openid)
		ssmgr.kickout(ss, result)
	end
end

function net.cs_create_role_req( ss, name )
	log_debug("cs_create_role_req", ss.openid, name)
	dbagent.ss_create_role_req(ss.openid, name)
end

function net.ss_create_role_rsp( result, openid, role )
	log_debug("ss_create_role_rsp", result, openid, role)
	local ss = ssmgr.find_by_openid(openid)
	if not ss then
		log_warn("ss_create_role_rsp ss not exist", openid)
		return
	end

	if result == errno.SUCCESS then
		ssmgr.bind_role(ss, role)
	end
	ss.cs_create_role_rsp(result, role)
end

function net.cs_select_role_req( ss, roleid )
	log_debug("cs_select_role_req", ss.openid, roleid)
	dbagent.ss_load_role_req(ss.openid, roleid)
end

function net.ss_load_role_rsp( result, openid, role )
	log_debug("ss_load_role_rsp", result, openid, role)
	local ss = ssmgr.find_by_openid(openid)
	if not ss then
		log_warn("ss_load_role_rsp ss not exist", openid)
		return
	end

	if result == errno.SUCCESS then
		ssmgr.bind_role(ss, role)
	end
	ss.cs_select_role_rsp(result, role)
end

function net.ss_save_role_rsp( result, openid, roleid )
	log_debug("ss_save_role_rsp", result, openid, roleid)
	if result ~= errno.SUCCESS then
		log_error("ss_save_role_rsp return failed", result, openid, roleid)
	end
end
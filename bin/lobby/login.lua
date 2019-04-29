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
	end
	ss.cs_login_rsp(flowid, result, account)
end

function net.cs_create_role_req( ss, flowid, name )
	log_info("cs_create_role_req", ss.number, flowid, name)
	dbagent.ss_create_role_req(flowid, ss.openid, name)
end

function net.ss_create_role_rsp(flowid, result, openid, role)
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
-- 登录逻辑

function net.cs_login_req(ss, flowid, openid, token)
	log_info("cs_login_req", ss.number, flowid, openid, token)
	dbagent.ss_login_req(flowid, ss.number, openid)
	ss.cs_login_rsp(flowid, err_code.SUCCESS)
end
-- 登录逻辑

function net.ss_login_req(ss, flowid, number, openid, svrid)
	log_info("ss_login_req", ss.number, flowid, number, openid)
	ss.ss_login_rsp(flowid, number, openid, svrid)
end
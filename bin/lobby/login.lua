-- 登录逻辑

function net.cs_login_req( ss, flowid, openid, token )
	log_info("cs_login_req", ss.number, flowid, openid, token)
	dbagent.ss_login_req(flowid, ss.number, openid, app.svrid())
end

function net.ss_login_rsp( flowid, number, openid, svrid )
	local ss = ssmgr.find_by_number(number)
	if not ss then
		return
	end

	if svrid == app.svrid() then
		ss.cs_login_rsp(flowid, err_code.SUCCESS)
	else
		ss.cs_login_rsp(flowid, err_code.FAILURE)
	end
end
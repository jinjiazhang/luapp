-- 登录逻辑

function net.ss_login_req(ss, flowid, number, openid, svrid)
	log_info("ss_login_req", ss.number, flowid, number, openid)
	local limit = string.format("openid='%s'", openid)
	local code, data = sqlpool.sql_select("tb_online", limit)
	if code < 0 then
		ss.ss_login_rsp(flowid, number, openid, -1)
		return
	end

	if data and app.time() - data.online < 30 then
		ss.ss_login_rsp(flowid, number, openid, data.svrid)
		return
	end

	if not data then
		data = { openid = openid, svrid = svrid, online = app.time() }
		if sqlpool.sql_insert("tb_online", data) < 0 then
			ss.ss_login_rsp(flowid, number, openid, -1)
			return
		end
	else
		-- TODO check magic
		data.online = app.time()
		if sqlpool.sql_update("tb_online", data, limit) < 0 then
			ss.ss_login_rsp(flowid, number, openid, -1)
			return
		end
	end

	ss.ss_login_rsp(flowid, number, openid, svrid)
end
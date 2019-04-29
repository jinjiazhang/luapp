-- 机器人脚本

function run( ... )
	run_test(login_flow, "test002", "ABCDEFG")
end

function run_test( f, ... )
	local co = coroutine.create(f)
	coroutine.resume(co, ...)
end

function check_result( proto, result )
	if result ~= errno.SUCCESS then
		local errmsg = string.format("proto: %s, result: %d", proto, result)
		error(errmsg, 2)
		return
	end
end

function login_flow( openid, token )
	local result, account = client.cs_login_req(openid, token)
	log_info("cs_login_req", result, proto.to_json(account))
	check_result("cs_login_req", result)

	if account.roleid == 0 then
		local name = string.format("name_%d", app.time())
		local result, role = client.cs_create_role_req(name)
		log_info("cs_create_role_req", result, proto.to_json(role))
		check_result("cs_create_role_req", result)
	else
		local result, role = client.cs_select_role_req(account.roleid)
		log_info("cs_select_role_req", result, proto.to_json(role))
		check_result("cs_select_role_req", result)
	end
end
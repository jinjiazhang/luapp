-- 机器人脚本

function run( ... )
	run_test(login_flow, "test002", "ABCDEFG")
end

function run_test( f, ... )
	local co = coroutine.create(f)
	coroutine.resume(co, ...)
end

function login_flow( openid, token )
	local result, account = client.cs_login_req(openid, token)
	log_info("cs_login_req", result, proto.to_json(account))

	if result ~= errno.SUCCESS then
		return
	end

	if account.roleid == 0 then
		local name = string.format("name_%d", app.time())
		local result, role = client.cs_create_role_req(name)
		log_info("cs_create_role_req", result, proto.to_json(role))
	end
end
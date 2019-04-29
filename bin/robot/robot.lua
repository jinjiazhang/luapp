-- 机器人脚本

function run( ... )
	run_test(login_flow, "openid_test001", "token_test001")
end

function run_test( f, ... )
	local co = coroutine.create(f)
	coroutine.resume(co, ...)
end

function login_flow( openid, token )
	local result = client.cs_login_req(openid, token)
	log_info("login_flow login", result)
end
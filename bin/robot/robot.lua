-- 机器人脚本
client = import("robot/client.lua")

function run( ... )
	test_login()
end

function test_login( ... )
	client.request_login("openid_test", "token_abc")

end
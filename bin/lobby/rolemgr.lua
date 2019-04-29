-- 会话管理器
module = "rolemgr"
openid_role_table = number_role_table or {}
roleid_role_table = number_role_table or {}

function find_by_openid( openid )
	return openid_role_table[openid]
end

function find_by_roleid( roleid )
	return roleid_role_table[roleid]
end

function on_login( ss, role )
	role.online = app.time()
end

function on_logout( ss, role )
	role.offline = app.time()
end
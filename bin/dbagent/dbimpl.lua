module = "dbimpl"

function increment_global_id(name, def_val)

end

function gen_unique_roleid()
    return increment_global_id("roleid", 10000)	
end

function get_online_info(openid)

end

function set_online_info(openid, svrid)

end

function clean_online_info(openid)

end

function load_account_data(openid)

end

function save_account_data(openid, account)

end

function load_role_data(roleid)

end

function save_role_data(roleid, role)

end
module = "dbimpl"

function increment_global_id(key, def_val)
    local code, result = mongodb.mongo_find_and_modify("global", {key = key}, {["$inc"] = {value = 1}})
    if code == 0 and result.value then
        return true, result.value.value
    end

    code, result = mongodb.mongo_insert("global", {key = key, value = def_val})
    if code == 0 then
        return true, def_val
    end

    code, result = mongodb.mongo_find_and_modify("global", {key = key}, {["$inc"] = {value = 1}})
    if code == 0 and result.value then
        return true, result.value.value
    end

    return false
end

function gen_unique_roleid()
    return increment_global_id("roleid", 10001)	
end

function insert_online_info(online)
    assert(online.openid ~= nil)
    local code, result = mongodb.mongo_insert("online", online)
    if code ~= 0 then
        log_error("insert_online_info failed", result)
        return false
    end
    return true
end

function get_online_info(openid)
    local code, result = mongodb.mongo_find("online", {openid = openid})
    if code ~= 0 then
        log_error("get_online_info failed", result)
        return false
    end
    assert(not result or result.openid == openid)
    return true, result
end

function set_online_info(openid, online)
    assert(online.openid == openid)
    local code, result = mongodb.mongo_replace("online", {openid = openid}, online)
    if code ~= 0 then
        log_error("set_online_info failed", result)
        return false
    end
    return true
end

function clean_online_info(openid)
    local code, result = mongodb.mongo_delete("online", {openid = openid})
    if code ~= 0 then
        log_error("clean_online_info failed", result)
        return false
    end
    return true
end

function create_new_account(account)
    assert(account.openid ~= nil)
    local code, result = mongodb.mongo_insert("account", account)
    if code ~= 0 then
        log_error("create_new_account failed", result)
        return false
    end
    return true
end

function load_account_data(openid)
    local code, result = mongodb.mongo_find("account", {openid = openid})
    if code ~= 0 then
        log_error("load_account_data failed", result)
        return false
    end
    assert(not result or result.openid == openid)
    return true, result
end

function save_account_data(openid, account)
    assert(account.openid == openid)
    local code, result = mongodb.mongo_replace("account", {openid = openid}, account)
    if code ~= 0 then
        log_error("save_account_data failed", result)
        return false
    end
    return true
end

function create_new_role(role)
    assert(role.roleid ~= nil)
    local code, result = mongodb.mongo_insert("role", role)
    if code ~= 0 then
        log_error("create_new_role failed", result)
        return false
    end
    return true
end

function load_role_data(roleid)
    local code, result = mongodb.mongo_find("role", {roleid = roleid})
    if code ~= 0 then
        log_error("load_role_data failed", result)
        return false
    end
    assert(not result or result.roleid == roleid)
    return true, result
end

function save_role_data(roleid, role)
    assert(role.roleid == roleid)
    local code, result = mongodb.mongo_replace("role", {roleid = roleid}, role)
    if code ~= 0 then
        log_error("save_role_data failed", result)
        return false
    end
    return true
end
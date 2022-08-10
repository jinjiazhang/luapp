module = "dbimpl"

function incr_global_id(name, def_val)
    local script = [[
        local val = redis.call("get", KEYS[1])
        if not val then
            redis.call("set", KEYS[1], ARGV[1])
            return ARGV[1]
        end
        return redis.call("incr", KEYS[1])
    ]]

    local key = string.format("global:%s", name)
    local status, val = redisdb.command("EVAL", script, 1, key, def_val)
    if status == 0 then
        return true, val
    end

    return false
end

function gen_unique_roleid()
    return incr_global_id("roleid", 10001)	
end

function insert_online_info(online)
    assert(online.openid ~= nil)
    local key = string.format("online:%s", online.openid)
    local value = json.encode(online)
    local status, errmsg = redisdb.command("SETNX", key, value)
    if status ~= 0 then
        log_error("insert_online_info failed", errmsg)
        return false
    end
    return true
end

function get_online_info(openid)
    local key = string.format("online:%s", openid)
    local status, value = redisdb.command("GET", key)
    if status ~= 0 then
        log_error("get_online_info failed", value)
        return false
    end

    if value == nil then
        return true, nil
    end

    local data = json.decode(value)
    assert(not data or data.openid == openid)
    return true, data
end

function set_online_info(openid, online)
    assert(online.openid == openid)
    local key = string.format("online:%s", openid)
    local value = json.encode(online)
    local status, errmsg = redisdb.command("SET", key, value)
    if status ~= 0 then
        log_error("set_online_info failed", errmsg)
        return false
    end
    return true
end

function clean_online_info(openid)
    local key = string.format("online:%s", openid)
    local status, errmsg = redisdb.command("DEL", key, value)
    if status ~= 0 then
        log_error("clean_online_info failed", errmsg)
        return false
    end
    return true
end

function create_new_account(account)
    assert(account.openid ~= nil)
    local key = string.format("account:%s", account.openid)
    local value = json.encode(account)
    local status, errmsg = redisdb.command("SETNX", key, value)
    if status ~= 0 then
        log_error("create_new_account failed", errmsg)
        return false
    end
    return true
end

function load_account_data(openid)
    local key = string.format("account:%s", openid)
    local status, value = redisdb.command("GET", key)
    if status ~= 0 then
        log_error("load_account_data failed", value)
        return false
    end

    if value == nil then
        return true, nil
    end

    local data = json.decode(value)
    assert(not data or data.openid == openid)
    return true, data
end

function save_account_data(openid, account)
    assert(account.openid == openid)
    local key = string.format("account:%s", openid)
    local value = json.encode(account)
    local status, errmsg = redisdb.command("SET", key, value)
    if status ~= 0 then
        log_error("save_account_data failed", errmsg)
        return false
    end
    return true
end

function create_new_role(role)
    assert(role.roleid ~= nil)
    local key = string.format("role:%s", role.roleid)
    local value = json.encode(role)
    local status, errmsg = redisdb.command("SETNX", key, value)
    if status ~= 0 then
        log_error("create_new_role failed", errmsg)
        return false
    end
    return true
end

function load_role_data(roleid)
    local key = string.format("role:%s", roleid)
    local status, value = redisdb.command("GET", key)
    if status ~= 0 then
        log_error("load_role_data failed", value)
        return false
    end

    if value == nil then
        return true, nil
    end

    local data = json.decode(value)
    assert(not data or data.roleid == roleid)
    return true, data
end

function save_role_data(roleid, role)
    assert(role.roleid == roleid)
    local key = string.format("role:%s", roleid)
    local value = json.encode(role)
    local status, errmsg = redisdb.command("SET", key, value)
    if status ~= 0 then
        log_error("save_role_data failed", errmsg)
        return false
    end
    return true
end
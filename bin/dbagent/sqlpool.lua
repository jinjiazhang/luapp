module = "sqlpool"

sqlpool_callbacks = sqlpool_callbacks or {}

function init( ... )
	setmetatable(_ENV, {__index = __index})
	mysql.parse("proto/db.proto")
	_sqlpool = mysql.create_pool()
	_sqlpool.on_respond = on_respond
	_sqlpool.connect(config.mysql_ip, config.mysql_user, config.mysql_pass, config.mysql_db, config.mysql_port)
	thread.start(init_schema)
end

function on_respond( token, ret_code, ... )
	-- log_debug("sqlpool.on_respond", token, ret_code, ...)
	if token == 0 and ret_code ~= 0 then -- connect fail
		log_info("sqlpool.on_connect fail", ret_code)
		return
	end

	local proc_func = sqlpool_callbacks[token]
	if proc_func then
		sqlpool_callbacks[token] = nil
		local status, errmsg = xpcall(proc_func, debug.traceback, ret_code, ...)
		if not status then
			log_error("sqlpool.on_respond xpcall fail", errmsg)
		end
	end
end

function __index( env, key )
	if _G[key] then
		return _G[key]
	elseif type(_sqlpool[key]) == 'function' then
		env[key] = create_co_func(key, _sqlpool[key])
		return env[key]
	end
end

function create_co_func( name, pool_func )
	return function(...)
		local token = pool_func(...)
		if not token then
			log_error("sqlpool.co_func call fail", name, ...)
			return -1
		end

		local co = coroutine.running()
		sqlpool_callbacks[token] = function(...)
			local status, errmsg = coroutine.resume(co, ...)
			if not status then
				log_error("sqlpool.co_func resume fail", name, errmsg)
			end
		end
		return coroutine.yield("EXIT")
	end
end

function init_schema(  )
	local code, results = sqlpool.sql_execute("show tables")
	if code < 0 then
		log_error("sqlpool.init_schema show tables fail")
		return
	end

	local need_tabls = {
		["tb_global"] = {primarys = {"name"}},
		["tb_online"] = {primarys = {"openid"}},
		["tb_name"] = {primarys = {"name"}},
		["tb_account"] = {primarys = {"openid"}},
		["tb_role"] = {primarys = {"roleid"}},
		["tb_profile"] = {primarys = {"roleid"}},
		["tb_room"] = {primarys = {"roomid"}},
	}

	results = results or {}
	for _, record in pairs(results) do
		for _, name in pairs(record) do
			if need_tabls[name] then
				need_tabls[name].exist =  true
			end
		end
	end

	for name, info in pairs(need_tabls) do
		if not info.exist then
			log_info("sqlpool.init_schema create table", name)
			if sqlpool.sql_create(name, table.unpack(info.primarys)) < 0 then
				log_error("sqlpool.init_schema create table fail")
				return
			end
		end
	end
end
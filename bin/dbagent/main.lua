_G.import = app.import
proto.parse("proto/ss.proto")
global = import("common/global.lua")
config = import("common/config.lua")
thread = import("common/thread.lua")
server = import("dbagent/server.lua")
redisdb = import("dbagent/redisdb.lua")
dbimpl = import("dbagent/redis_impl.lua")
-- mongodb = import("dbagent/mongodb.lua")
-- dbimpl = import("dbagent/mongo_impl.lua")
ssmgr = import("dbagent/ssmgr.lua")
login = import("dbagent/login.lua")

app.init = function ( ... )
	config.init()
	server.init()
	redisdb.init()
	-- mongodb.init()
end

app.proc = function ( ... )

end

app.tick = function ( ... )
	app.reload()
end

app.idle = function ( ... )

end
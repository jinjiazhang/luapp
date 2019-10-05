_G.import = app.import
proto.parse("proto/ss.proto")
config = import("config/config.lua")
copool = import("dbagent/copool.lua")
server = import("dbagent/server.lua")
mongodb = import("dbagent/mongodb.lua")
dbimpl = import("dbagent/dbimpl.lua")
ssmgr = import("dbagent/ssmgr.lua")
login = import("dbagent/login.lua")

app.init = function ( ... )
	config.init()
	server.init()
	mongodb.init()
end

app.proc = function ( ... )

end

app.tick = function ( ... )
	app.reload()
end

app.idle = function ( ... )

end
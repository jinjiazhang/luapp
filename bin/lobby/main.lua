_G.import = app.import
proto.parse("protos/cs.proto")
proto.parse("protos/ss.proto")
server = import("lobby/server.lua")
dbagent = import("lobby/dbagent.lua")
airport = import("lobby/airport.lua")
ssmgr = import("lobby/ssmgr.lua")
login = import("lobby/login.lua")

app.init = function ( ... )
	server.init()
	dbagent.init()
	airport.init()
end

app.proc = function ( ... )

end

app.tick = function ( ... )
	app.reload()
end

app.idle = function ( ... )

end
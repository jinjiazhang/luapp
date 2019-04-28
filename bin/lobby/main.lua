_G.import = app.import
proto.parse("proto/cs.proto")
proto.parse("proto/ss.proto")
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
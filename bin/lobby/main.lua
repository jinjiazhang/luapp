_G.import = app.import
proto.parse("proto/ss.proto")
config = import("common/config.lua")
dbagent = import("common/dbagent.lua")
airport = import("common/airport.lua")
server = import("lobby/server.lua")
login = import("lobby/login.lua")
room = import("lobby/room.lua")
ssmgr = import("lobby/ssmgr.lua")
rolemgr = import("lobby/rolemgr.lua")


app.init = function ( ... )
	config.init()
	server.init()
	dbagent.init()
	airport.init()
end

app.proc = function ( ... )

end

app.tick = function ( ... )
	app.reload()
	ssmgr.tick()
	rolemgr.tick()
end

app.idle = function ( ... )

end
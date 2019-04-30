_G.import = app.import
proto.parse("proto/ss.proto")
server = import("lobby/server.lua")
dbagent = import("lobby/dbagent.lua")
airport = import("lobby/airport.lua")
login = import("lobby/login.lua")
room = import("lobby/room.lua")
ssmgr = import("lobby/ssmgr.lua")
rolemgr = import("lobby/rolemgr.lua")


app.init = function ( ... )
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
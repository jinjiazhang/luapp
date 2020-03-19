_G.import = app.import
proto.parse("proto/ss.proto")
config = import("config/config.lua")
dbagent = import("gamesvr/dbagent.lua")
airport = import("gamesvr/airport.lua")
roommgr = import("gamesvr/roommgr.lua")

app.init = function ( ... )
	config.init()
	dbagent.init()
	airport.init()
end

app.proc = function ( ... )

end

app.tick = function ( ... )
	app.reload()
	roommgr.tick()
end

app.idle = function ( ... )

end
_G.import = app.import
proto.parse("proto/ss.proto")
global = import("common/global.lua")
config = import("common/config.lua")
dbagent = import("common/dbagent.lua")
airport = import("common/airport.lua")
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
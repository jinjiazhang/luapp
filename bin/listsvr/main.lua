_G.import = app.import
proto.parse("proto/ss.proto")
config = import("config/config.lua")
dbagent = import("listsvr/dbagent.lua")
airport = import("listsvr/airport.lua")

app.init = function ( ... )
	config.init()
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
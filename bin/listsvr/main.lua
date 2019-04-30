_G.import = app.import
proto.parse("proto/ss.proto")
dbagent = import("listsvr/dbagent.lua")
airport = import("listsvr/airport.lua")

app.init = function ( ... )
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
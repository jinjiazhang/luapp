_G.import = app.import
proto.parse("proto/cs.proto")
proto.parse("proto/ss.proto")
dbagent = import("roomsvr/dbagent.lua")
airport = import("roomsvr/airport.lua")

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
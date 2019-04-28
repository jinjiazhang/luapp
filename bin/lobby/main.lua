_G.import = app.import
server = import("lobby/server.lua")
dbagent = import("lobby/dbagent.lua")
airport = import("lobby/airport.lua")

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
_G.import = app.import
dbagent = import("lobby/dbagent.lua")
airport = import("lobby/airport.lua")

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
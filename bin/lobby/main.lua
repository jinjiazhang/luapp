_G.import = app.import
airport = import("lobby/airport.lua")

app.init = function ( ... )
	airport.init()
end

app.proc = function ( ... )

end

app.tick = function ( ... )
	app.reload()
end

app.idle = function ( ... )

end
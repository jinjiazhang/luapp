_G.import = app.import
client = import("robot/client.lua")

app.init = function ( ... )
	client.init()
end

app.proc = function ( ... )

end

app.tick = function ( ... )
	app.reload()
end

app.idle = function ( ... )

end
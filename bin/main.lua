_G.import = app.import
listen = import("scripts/listen.lua")
connect = import("scripts/connect.lua")

app.init = function ( ... )
	log_info("app.time", app.time())
	listen.init()
	connect.init()
end

app.proc = function ( ... )

end

app.tick = function ( ... )

end

app.idle = function ( ... )

end
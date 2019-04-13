_G.import = app.import
timeout = import("scripts/timeout.lua")
listen = import("scripts/listen.lua")
connect = import("scripts/connect.lua")
httpreq = import("scripts/httpreq.lua")
rdsclient = import("scripts/rdsclient.lua")

app.init = function ( ... )
	log_info("app.time", app.time())
	proto.parse("game.proto")
	timeout.init()
	listen.init()
	connect.init()
	httpreq.init()
	rdsclient.init()
end

app.proc = function ( ... )

end

app.tick = function ( ... )

end

app.idle = function ( ... )

end
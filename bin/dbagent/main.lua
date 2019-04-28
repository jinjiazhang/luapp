_G.import = app.import
listen = import("dbagent/listen.lua")
sqlpool = import("dbagent/sqlpool.lua")
rdsclient = import("dbagent/rdsclient.lua")

app.init = function ( ... )
	listen.init()
	sqlpool.init()
	rdsclient.init()
end

app.proc = function ( ... )

end

app.tick = function ( ... )
	app.reload()
end

app.idle = function ( ... )

end
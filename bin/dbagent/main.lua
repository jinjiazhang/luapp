_G.import = app.import
proto.parse("proto/ss.proto")
copool = import("dbagent/copool.lua")
server = import("dbagent/server.lua")
sqlpool = import("dbagent/sqlpool.lua")
rdsclient = import("dbagent/rdsclient.lua")
ssmgr = import("dbagent/ssmgr.lua")
login = import("dbagent/login.lua")

app.init = function ( ... )
	server.init()
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
_G.import = app.import
proto.parse("proto/ss.proto")
global = import("common/global.lua")
config = import("common/config.lua")
dbagent = import("common/dbagent.lua")
remote = import("common/remote.lua")
roommgr = import("gamesvr/roommgr.lua")

app.init = function ( ... )
	config.init()
	dbagent.init()
	remote.init()
end

app.proc = function ( ... )

end

app.tick = function ( ... )
	app.reload()
	roommgr.tick()
end

app.idle = function ( ... )

end
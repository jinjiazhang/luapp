_G.import = app.import
proto.parse("proto/ss.proto")
global = import("common/global.lua")
config = import("common/config.lua")
dbagent = import("common/dbagent.lua")
remote = import("common/remote.lua")
unique = import("explorer/unique.lua")
roommgr = import("explorer/roommgr.lua")
balance = import("explorer/balance.lua")

app.init = function ( ... )
	config.init()
	dbagent.init()
	remote.init()
end

app.proc = function ( ... )

end

app.tick = function ( ... )
	app.reload()
end

app.idle = function ( ... )

end
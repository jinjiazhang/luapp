_G.import = app.import
proto.parse("proto/ss.proto")
global = import("common/global.lua")
config = import("common/config.lua")
gateway = import("gateway/gateway.lua")

app.init = function ( ... )
	config.init()
	gateway.init()
end

app.proc = function ( ... )

end

app.tick = function ( ... )
	app.reload()
end

app.idle = function ( ... )

end
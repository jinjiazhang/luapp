_G.import = app.import
proto.parse("proto/ss.proto")
global = import("common/global.lua")
config = import("common/config.lua")
router = import("router/router.lua")

app.init = function ( ... )
	config.init()
	router.init()
end

app.proc = function ( ... )

end

app.tick = function ( ... )
	app.reload()
end

app.idle = function ( ... )

end
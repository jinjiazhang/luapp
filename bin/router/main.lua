_G.import = app.import
router = import("router/router.lua")

app.init = function ( ... )
	router.init()
end

app.proc = function ( ... )

end

app.tick = function ( ... )
	app.reload()
end

app.idle = function ( ... )

end
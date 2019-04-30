_G.import = app.import
proto.parse("proto/cs.proto")
config = import("config/config.lua")
notify = import("robot/notify.lua")
client = import("robot/client.lua")
robot = import("robot/robot.lua")

app.init = function ( ... )
	config.init()
	client.init()
	robot.run()
end

app.proc = function ( ... )

end

app.tick = function ( ... )
	app.reload()
end

app.idle = function ( ... )

end
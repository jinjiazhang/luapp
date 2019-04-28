_G.import = app.import
proto.parse("protos/cs.proto")
client = import("robot/client.lua")
robot = import("robot/robot.lua")

app.init = function ( ... )
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
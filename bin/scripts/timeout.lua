-- 定时器示例

function init( ... )
	local start = app.mstime()
	local tid1 = timer.setup(1, function( tid )
		log_info(string.format("timer[%d] is timeout, cost %dms", tid, app.mstime() - start))
	end)

	local tid2 = timer.setup(2, function( tid )
		log_info(string.format("timer[%d] is timeout, cost %dms", tid, app.mstime() - start))
	end)
	timer.cancel(tid2)

	local tid3 = timer.setup(3, function( tid )
		log_info(string.format("timer[%d] is timeout, cost %dms", tid, app.mstime() - start))
	end)
	timer.update(tid3, 2)
end
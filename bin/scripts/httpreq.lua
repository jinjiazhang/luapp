-- http请求示例

function init( ... )
	local start = app.mstime()
	http.async_get("http://www.baidu.com", function( code, data )
		log_info(string.format("http req cost %dms, code %d, data:%s", app.mstime() - start, code, data))
	end)
end
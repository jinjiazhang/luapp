-- http请求示例

function init( ... )
	local start = app.mstime()
	http.async_get("http://httpbin.org/get?a=1&b=2", function( code, data )
		log_info(string.format("http req cost %dms, code %d, data=%s", app.mstime() - start, code, data))
	end)
end
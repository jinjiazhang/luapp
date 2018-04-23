const char* assist_code = u8R"__(
package.path = package.path.."; .\\?"

app.files = {}

app.load = function ( name )
	local node = app.files[name]
	local env = node and node.env
	local chunk, message = loadfile(name, "bt", env)
	if not chunk then
		return false, message
	end

	local status, message = pcall(chunk)
	if not status then
		return false, message
	end

	if env and env.module then
		_G[env.module] = env
	end
	return true
end

app.import = function ( name )
	local node = app.files[name]
	if node and node.env then
		return node.env
	end

	local env = {}
	setmetatable(env, {__index = _G})
	app.files[name] = {
		env = env,
		name = name,
		time = sys.filetime(name),
		md5hex = sys.md5file(name),
	}

	local status, message = app.load(name)
	if not status then
		log_error(string.format("import file fail: %s", name))
		error(message)
		return nil
	end
	log_info(string.format("import file succeed: %s", name))
	return env
end

app.reload = function (  )
	for name, node in pairs(app.files) do
		local time = sys.filetime(name)
		if time == node.time then
			goto continue
		end

		node.time = time
		local md5hex = sys.md5file(name)
		if md5hex == node.md5hex then
			goto continue
		end

		local status, message = app.load(name)
		if not status then
			log_error(string.format("import file fail: %s", name))
			log_error(message)
			goto continue
		end
		log_info(string.format("import file succeed: %s", name))

		::continue::
	end
end
)__";
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

    local env = {_G = _G}
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

app.tostring = function(value, layer)
    if type(value) ~= 'table' then
        return tostring(value)
    end
    layer = layer or 1
    if layer >= 10 then
    	return '{...}'
    end
    local s = '{\n'
    for k, v in pairs(value) do
        s = s..string.rep('    ', layer)
        if type(k) == 'string' then
            s = s..k
        else
            s = s..'['
            s = s..tostring(k)
            s = s..']'
        end
        s = s..' = '
        if v == _G then
            s = s..'_G'
        elseif v == value then
            s = s..'this'
        elseif v == package then
            s = s..'package'
        elseif type(v) == 'table' then
            s = s..app.tostring(v, layer+1)
        elseif type(v) == 'string' then
            s = s..string.format("%q",v)
        else
            s = s..tostring(v)
        end
        s = s..',\n'
    end
    s = s..string.rep('    ', layer-1)
    s = s..'}'
    return s
end

timer.setups = {}

timer.setup = function( second, loop, closure )
    local tid = timer.insert(second, loop)
    timer.setups[tid] = {
        loop = loop,
        closure = closure,
    }
    return tid
end

timer.cancel = function( tid )
    timer.setups[tid] = nil    
    return timer.remove(tid)
end

timer.update = function( tid, second )
    return timer.change(tid, second)
end

timer.timeout = function( tid )
    local setup = timer.setups[tid]
    if not setup.loop then
        timer.setups[tid] = nil
    end
    setup.closure(tid)
end

http.responses = {}

http.async_get = function( url, closure )
    local token = http.get(url)
    http.responses[token] = closure
    return token
end

http.async_post = function( url, data, closure )
    local token = http.post(url, data)
    http.responses[token] = closure
    return token
end

http.respond = function( token, code, data )
    local closure = http.responses[token]
    http.responses[token] = nil
    closure(code, data)
end

)__";
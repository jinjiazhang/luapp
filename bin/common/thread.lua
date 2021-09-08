module = "thread"

coroutine_pool = setmetatable({}, { __mode = "kv" })

function create( f )
    local co = table.remove(coroutine_pool)
    if co == nil then
        co = coroutine.create(function( ... )
            f(...)
            while true do
                f = nil
                coroutine_pool[#coroutine_pool+1] = co
                f = coroutine.yield("EXIT")
                f(coroutine.yield("INIT"))
            end
        end)
    else
        local status, magic = coroutine.resume(co, f)
        assert(status and magic == "INIT")
    end
    return co
end

function start( f, ... )
    local status, errmsg = coroutine.resume(create(f), ...)
    if not status then
        log_error("thread.start", errmsg)
    end
end
-- origin lua coroutine module
module = "copool"

local coroutine_resume = coroutine.resume
local coroutine_yield = coroutine.yield
local coroutine_status = coroutine.status
local coroutine_running = coroutine.running
local coroutine_create = coroutine.create

local coroutine_pool_status_suspended = "SUSPEND"

coroutine_pool_max_size = 1024
coroutine_pool = coroutine_pool or {} 
coroutine_used = coroutine_used or setmetatable({}, { __mode = "kv" })

function _coroutine_loop_tailcall(co) 
    assert(coroutine_used[co])
    coroutine_used[co] = nil

    local coroutine_pool_count = #coroutine_pool
    if coroutine_pool_count >= coroutine_pool_max_size then
        return
    end

    -- recycle co into pool
    coroutine_pool[coroutine_pool_count + 1] = co
    
    -- recv new main function f
    local f = coroutine_yield(coroutine_pool_status_suspended)
    assert(coroutine_used[co] == nil)
    coroutine_used[co] = true
    f(coroutine_yield(coroutine_pool_status_suspended))

    return _coroutine_loop_tailcall(co)
end

local function _coroutine_loop() 
    local co, main = coroutine_running()
    assert(not main)
    coroutine_used[co] = true
    return _coroutine_loop_tailcall(co)
end

local function _coroutine_pool_create() 
    local co = coroutine_create(_coroutine_loop)
    assert(coroutine_resume(co))
    co = table.remove(coroutine_pool)
    assert(co)
    return co
end

function create(f)
    local co = table.remove(coroutine_pool)
    if co == nil then
        co = _coroutine_pool_create()
    end
    
    local ok, ret = coroutine_resume(co, f)
    assert(ok and ret == coroutine_pool_status_suspended)
    return co
end

local function check_ok(msg, ok, ...)
    if not ok then
        print(msg, ...)
    end
    return ok, ...
end

function fork(f, ...)
    return check_ok("fork check_ok:", coroutine_resume(create(f), ...))
end

function status()
    local coroutine_used_debug = {}
    for co, _ in pairs(coroutine_used) do
        table.insert(coroutine_used_debug, { 
            co = tostring(co), 
            status = coroutine_status(co), 
            traceback = debug.traceback(co), 
        })
    end

    local status = {
        coroutine_used = coroutine_used_debug, 
        coroutine_used_count = #coroutine_used_debug, 
        coroutine_pool_count = #coroutine_pool
    }
    return status
end

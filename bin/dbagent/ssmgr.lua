module = "ssmgr"

netid_session_table = netid_session_table or {}

function find_by_netid( netid )
	return netid_session_table[netid]
end

function __index_ss( ss, key )
	if proto.exist(key) then
		ss[key] = function ( ... )
			net.call(ss.netid, key, ...)
		end
		return ss[key]
	end
end

function on_start( netid )
	local ss = { netid = netid }
	setmetatable(ss, {__index = __index_ss})
	netid_session_table[netid] = ss
end

function on_stop( netid )
	netid_session_table[netid] = nil
end

function on_call( netid, proto, ... )
	local ss = find_by_netid(netid)
	if not ss then
		log_error("ssmgr.on_call session not found", netid)
		return
	end

	copool.fork(proc_call, ss, proto, ...)
end

function proc_call( ss, proto, ... )
	local proc_func = net[proto]
	if not proc_func then
		log_error("ssmgr.on_call proc_func not found", proto)
		return
	end

	proc_func(ss, ...)
end
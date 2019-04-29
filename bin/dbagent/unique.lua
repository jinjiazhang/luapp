-- 生成全局唯一id
module = "unique"

instance = app.svrid() % 256
sequence = sequence or 1

function gen_seq(  )
	sequence = (sequence + 1) % 256
	return sequence
end

function gen_stamp(  )
	return app.mstime() - 1555555555555
end

function gen_magic(  )
	return gen_stamp() * 65536 + gen_seq() * 256 + instance
end

function gen_flowid(  )
	return gen_stamp() * 65536 + gen_seq() * 256 + instance
end

function gen_roomid(  )
	return gen_stamp() * 65536 + gen_seq() * 256 + instance
end

function gen_roleid(  )
	return gen_stamp() * 65536 + gen_seq() * 256 + instance
end
module = "global"

function get_area_id( svrid )
	return (svrid >> 24) % 256
end
_G.get_area_id = get_area_id

function get_zone_id( svrid )
	return (svrid >> 16) % 256
end
_G.get_zone_id = get_zone_id

function get_func_id( svrid )
	return (svrid >> 8) % 256
end
_G.get_func_id = get_func_id

function get_inst_id( svrid )
	return (svrid >> 0) % 256
end
_G.get_inst_id = get_inst_id

function svrid_itos( svrid )
	return string.format("%d.%d.%d.%d",
		get_area_id( svrid ),
		get_zone_id( svrid ),
		get_func_id( svrid ),
		get_inst_id( svrid ))
end
_G.svrid_itos = svrid_itos
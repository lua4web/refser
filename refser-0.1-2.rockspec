package = "refser"
version = "0.1-1"
source = {
	url = "git://github.com/lua4web/refser.git"
}
description = {
	summary = "Fast serialization of tables with references",
	detailed = [[refser is similar to identity-preserving table serialization by Metalua, but it is much faster as it is written in C. ]],
	license = "MIT/X11"
}
dependencies = {
	"lua ~> 5.1"
}
build = {
	type = "builtin",
	modules = {
		crefser = {
			sources = {"crefser.c", "fixbuf.c"}
		},
		refser = "refser.lua"
	}
}


package = "refser"
version = "git-1"
source = {
	url = "git://github.com/lua4web/refser.git"
}
description = {
	summary = "Fast serialization of tables with references",
	detailed = [[refser allows to save primitive Lua types into strings and load them back. 
refser is similar to identity-preserving table serialization by Metalua, but it is much faster as it is written in C++. 
]],
	homepage = "http://github.com/lua4web/refser",
	license = "MIT/X11"
}
dependencies = {
	"lua >= 5.1, < 5.3",
	"luarocks-build-cpp"
}
build = {
	type = "cpp",
	modules = {
		refser = "refser.lua",
		["refser.crefser"] = {
			sources = {
				"src/crefser.cpp",
				"src/lua.cpp",
				"src/writer.cpp",
				"src/fixbuf.cpp",
				"src/saver.cpp",
				"src/loader.cpp"
			},
			incdirs = "src/"
		},
		["refser.auxiliary"] = {
			sources = {
				"src/auxiliary/auxiliary.c"
			},
			incdirs = "src/auxiliary/"
		}
	},
	copy_directories = {}
}

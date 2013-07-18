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
	"lua >= 5.1, < 5.3"
}
build = {
	type = "builtin",
	modules = {
		crefser = {
			sources = {
				"src/crefser.cpp",
				"src/luapp.cpp",
				"src/writer.cpp",
				"src/fixbuf.cpp",
				"src/filewriter.cpp",
				"src/saver.cpp",
				"src/loader.cpp"
			},
			incdirs = {
				"src/"
			},
			libraries = "stdc++"
		},
		refser = "refser.lua"
	},
	platforms = {
		win32 = {
			modules = {
				crefser = {
					libraries = {}
				}
			}
		}
	}
}


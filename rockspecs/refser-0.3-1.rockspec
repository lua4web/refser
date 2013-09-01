package = "refser"
version = "0.3-1"
source = {
	url = "git://github.com/lua4web/refser.git",
	tag = "v0.3"
}
description = {
	summary = "yet another Lua table serialization library",
	detailed = [[refser is yet another Lua table serialization library mostly written in C++. 
refser handles recursive tables and cross-references correctly. 
It has options to restrict saved and loaded data at run-time and options to persist context across several operations. 
]],
	homepage = "http://github.com/lua4web/refser",
	license = "MIT/X11"
}
dependencies = {
	"lua >= 5.1, < 5.3",
	-- "30log >= 0.6", -- doesn't work wirh current LR
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
		}
	},
	copy_directories = {}
}


local path = "/usr/lib/libluabsd.so"
local func = "luaopen_bsd"
local f = assert(package.loadlib(path, func))
return f()

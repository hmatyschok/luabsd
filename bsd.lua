
local path = "/usr/lib/libluabsd.so"
local func = "luaopen_bsd"
local f = assert(loadlib(path, func))
local function ret(s, ...)
    if s then
        return unpack(arg)
    end
    error(path .. ": " .. func .. " failed.")
end
return ret(pcall(f))

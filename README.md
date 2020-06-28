<pre><code>
libluabsd - toolbox for implementing web-based software for embedded systems
============================================================================

This library provides an easy customizable interface as extension of Lua
language against APIs those are common at the FreeBSD Operating System.

As an example, a database may created, as described in db(3):

    local lib = require("bsd")

    local _fname = "example.db"
    local _flags = bit32.bor(
        lib.fcntl.O_CREAT,
        lib.fcntl.O_TRUNC,
        lib.fcntl.O_RDWR
    )
    local _mode = bit32.bor(
        lib.sys_stat.S_IRUSR,
        lib.sys_stat.S_IWUSR,
        lib.sys_stat.S_IRGRP,
        lib.sys_stat.S_IWGRP,
        lib.sys_stat.S_IROTH,
        lib.sys_stat.S_IWOTH
    )
    local _type = lib.db.DB_BTREE

    local db = lib.db.dbopen(_fname, _flags, _mode, _type)

Therefore

    local key = lib.uuid.uuidgen()
    local value = "Hello world!"

    db:put(key, value, lib.db.R_NOOVERWRITE)

    local err, record = db:get(key, 0)

and so on. A callout may implemented as follows:

    local expired = false

    local function callback ()
        expired = true
        print("Hello world!")
    end

    lib.sys_time.setitimer(lib.sys_time.ITIMER_REAL, 3, callback)

    while true do -- yes, this is bad style ^^
        if expired then
            break
        end
    end

</code></pre>

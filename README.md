<pre><code>
libluabsd - toolbox for implementing (web-based) software for embedded systems
==============================================================================

This library provides an easy customizable interface as extension of Lua
language against APIs those are common on unixoid Operating Systems (e. g.
the FreeBSD OS).

As an example, a database may created, as described in db(3):

    local lib = require("bsd")

    local _fname = "example.db"
    local _flags = bit32.bor(
        lib.fcntl.O_CREAT,
        lib.fcntl.O_TRUNC,
        lib.fcntl.O_RDWR
    )
    local _mode = bit32.bor(
        lib.sys.stat.S_IRUSR,
        lib.sys.stat.S_IWUSR,
        lib.sys.stat.S_IRGRP,
        lib.sys.stat.S_IWGRP,
        lib.sys.stat.S_IROTH,
        lib.sys.stat.S_IWOTH
    )
    local _type = lib.db.DB_BTREE

    local db = lib.db.dbopen(_fname, _flags, _mode, _type)

Therefore

    local key = lib.uuid.uuidgen()
    local value = "Hello world!"

    db:put(key, value, lib.db.R_NOOVERWRITE)

    local err, record = db:get(key, 0)

and so on. A callout may implemented as follows:

    expired = false
    
    local function event()
        print("Hello world!")
        expired = true;
    end
    
    tv = bsd.sys.time.StructTimeSpec()
    tv:set_tv_sec(3)
    
    it1 = bsd.sys.time.StructItimerVal()
    it1:set_it_value(tv)
    
    it2 = bsd.sys.time.StructItimerVal()
    
    err, msg = bsd.sys.time.setitimer(lib.sys.time.ITIMER_REAL, it1, it2, event)

    while true do -- do something
        if expired then
            break
        end
    end
    
    print("setitimer()", err, msg)

</code></pre>
